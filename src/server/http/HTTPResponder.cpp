//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "utils/intToString.hpp"
#include "utils/strdup.hpp"
#include "utils/ErrorThrow.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/http/HTTPParser.hpp"
#include "env/ENVBuilder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <dirent.h>
#include <unistd.h>
#include "utils/Uri.hpp"
#include "server/http/HTTPMimeTypes.hpp"

using namespace NotApache;

void HTTPResponder::generateAssociatedResponse(HTTPClient &client) {
	if (client.responseState == FILE) {
		client.data.response.setResponse(
			client.data.response.builder
			.setHeader("Server", "Not-Apache") // TODO remove
			.setDate() // TODO remove
			.setHeader("Connection", "Close") // TODO remove
			.setBody(client.data.response.getAssociatedDataRaw())
			.build()
		);
		return;
	} else if (client.responseState == PROXY) {
		client.data.response.setResponse(
			ResponseBuilder(client.proxy->response.data)
			.build()
		);
		return;
	}
	else if (client.responseState == CGI) {
		client.data.response.setResponse(
			ResponseBuilder()
			.setBody(client.cgi->response.data.data)
			.build()
		);
		return;
	}	
}

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage) {
	handleError(client, server, 0, code, doErrorPage);
}

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route, int code, bool doErrorPage) {
	// Request authentication
	if (code == 401 && route)
		client.data.response.builder.setHeader("WWW-AUTHENTICATE", "Basic realm=\"Not-Apache\"");

	// allow header in 405
	if (code == 405 && route) {
		client.data.response.builder.setAllowedMethods(route->getAllowedMethods());
	}

	// handle error pages
	if (doErrorPage && server != 0 && !server->getErrorPage(code).empty()) {
		struct ::stat errorPageData = {};
		utils::Uri errorPageFile(server->getErrorPage(code));
		if (::stat(errorPageFile.path.c_str(), &errorPageData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, server, route, 500, false);
				return;
			}
		}
		if (S_ISREG(errorPageData.st_mode)) {
			prepareFile(client, *server, *route, errorPageFile, code);
			return;
		}
	}

	// generate error page
	client.data.response.builder
		.setStatus(code)
		.setHeader("Server", "Not-Apache") // TODO remove
		.setDate() // TODO remove
		.setHeader("Connection", "Close") // TODO remove
		.setHeader("Content-Type", "text/html");

	std::map<int,std::string>::const_iterator statusIt = ResponseBuilder::statusMap.find(code);
	std::string text = statusIt == ResponseBuilder::statusMap.end() ? "Internal server error!" : statusIt->second;
	client.data.response.builder.setBody(std::string("<h1>") + utils::intToString(code) + "</h1><p>" + text + "</p>");
	client.data.response.setResponse(client.data.response.builder.build());
}

void HTTPResponder::serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct stat &directoryStat, const std::string &d) {
	// check index
	utils::Uri dirPath = d;
	if (!route.getIndex().empty()) {
		struct ::stat indexData = {};
		utils::Uri indexFile = d;
		indexFile.appendPath(route.getIndex());
		if (::stat(indexFile.path.c_str(), &indexData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, &server, 500);
				return;
			}
		}

		// index file exists, serve it
		if (S_ISREG(indexData.st_mode)) {
			prepareFile(client, server, route, indexData, indexFile);
			return;
		}
	}

	// not index, handle directory listing
	if (route.isDirectoryListing()) {
		DIR *dir = ::opendir(dirPath.path.c_str());
		if (dir == 0) {
			handleError(client, &server, 500);
			return;
		}
		dirent *dirEntry;
		utils::Uri uri = client.data.request.data.uri;
		std::string str = "<h1>";
		str += uri.path + "</h1><ul>";
		while ((dirEntry = ::readdir(dir)) != 0) {
			utils::Uri path = uri;
			path.appendPath(dirEntry->d_name);
			str += "<li><a href=\"";
			str += path.path;
			str += "\">";
			if (dirEntry->d_type == DT_DIR) {
				str += "DIR ";
			}
			str += dirEntry->d_name;
			str += "</a></li>";
		}
		str += "</ul>";
		::closedir(dir);

		ResponseBuilder builder;
		builder
		.setModifiedDate(directoryStat.STAT_TIME_FIELD)
		.setHeader("Content-Type", "text/html");

		// add OPTIONS specific header
		if (client.data.request.data.method == OPTIONS) {
			builder.setAllowedMethods(route.getAllowedMethods());
		}

		// send request for methods that dont send a body
		if (client.data.request.data.method == HEAD || client.data.request.data.method == OPTIONS) {
			builder.removeHeader("CONTENT-LENGTH");
			client.data.response.setResponse(builder.build());
			return;
		}

		builder.setBody(str);
		client.data.response.setResponse(
			builder.build()
		);
		return;
	}

	// normal directory handler
	handleError(client, &server, 403);
}

void HTTPResponder::prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const utils::Uri &file, int code) {
	FD fileFd = ::open(file.path.c_str(), O_RDONLY);
	if (fileFd == -1) {
		HTTPResponder::handleError(client, &server, 500);
		return;
	}

	client.data.response.builder.setHeader("Content-Type", MimeTypes::getMimeType(file.getExt()));
	client.data.response.builder.setStatus(code);

	// add OPTIONS specific header
	// TODO options for * uri
	if (client.data.request.data.method == OPTIONS) {
		client.data.response.builder.setAllowedMethods(route.getAllowedMethods());
	}

	// send request for methods that dont send a file
	if (client.data.request.data.method == HEAD || client.data.request.data.method == OPTIONS) {
		client.data.response.builder.removeHeader("CONTENT-LENGTH");
		client.data.response.setResponse(client.data.response.builder.build());
		return;
	}

	// send file
	if (client.data.request.data.method == GET || client.data.request.data.method == POST) {
		client.addAssociatedFd(fileFd);
		client.responseState = NotApache::FILE;
		client.connectionState = ASSOCIATED_FD;
		return;
	}
}

void HTTPResponder::prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct ::stat &buf, const utils::Uri &file, int code) {
	client.data.response.builder.setModifiedDate(buf.STAT_TIME_FIELD);
	prepareFile(client, server, route, file, code);
}

void HTTPResponder::serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f) {
	struct ::stat buf = {};

	// check autorization
	if (!route.getAuthBasic().empty()) {
		std::map<std::string, std::string>::iterator it = client.data.request.data.headers.find("AUTHORIZATION");
		if (it == client.data.request.data.headers.end()) {
			handleError(client, &server, &route, 401);
			return ;
		}
		else {
			try {
				if (!checkCredentials(route.getAuthBasicUserFile(), it->second)) {
					handleError(client, &server, &route, 403);
					return ;
				}
			}
			catch (const std::exception& e) {
				globalLogger.logItem(logger::ERROR, std::string("File serving error: ") + e.what());
				handleError(client, &server, &route, 500);
			}
		}
	}

	// get file data
	utils::Uri file = f;
	if (::stat(file.path.c_str(), &buf) == -1) {
		if (errno == ENOENT || errno == ENOTDIR)
			handleError(client, &server, &route, 404);
		else
			handleError(client, &server, &route, 500);
		return;
	}

	// check for directory
	if (S_ISDIR(buf.st_mode)) {
		serveDirectory(client, server, route, buf, file.path);
		return;
	}
	else if (!S_ISREG(buf.st_mode)) {
		handleError(client, &server, &route, 403);
		return;
	}

	// serve the file
	if (route.shouldDoCgi() && !route.getCgiExt().empty() && file.getExt() == route.getCgiExt()) {
		globalLogger.logItem(logger::DEBUG, "Handling cgi request");
		// handle cgi
		runCGI(client, f, route.getCgi());
		return ;
	}
	prepareFile(client, server, route, buf, file);
}

bool HTTPResponder::checkCredentials(const std::string& authFile, const std::string& credentials) {
	FD fd;
	int ret = 1;
	char buf[255];
	std::string fileContent; // TODO parse file?

	// open and read from "password database"
	fd = ::open(authFile.c_str(), O_RDONLY);
	if (fd == -1)
		ERROR_THROW(OpenFail());
	while ((ret = ::read(fd, &buf, sizeof(buf))) > 0) {
		buf[ret] = '\0';
		fileContent += buf;
		if (fileContent.size() > 10000)
			ERROR_THROW(MaxFileSize());
	}
	if (ret == -1)
		ERROR_THROW(ReadFail());

	// split per user
	std::vector<std::string> userPasswordPair = utils::split(fileContent, "\n");
	
	// Check header
	if (credentials.find("Basic ", 0, 6) != 0)
		ERROR_THROW(AuthHeader());

	// Find match
	for (size_t i = 0; i < userPasswordPair.size(); ++i) {
		if (utils::base64_decode(credentials.substr(6)) == userPasswordPair[i])
			return true;
	}
	return false;
}

void HTTPResponder::generateResponse(HTTPClient &client) {
	if (client.data.request.data.parseStatusCode != 200) {
		// error responses if parsing failed
		handleError(client, 0, client.data.request.data.parseStatusCode);
		return;
	}
	std::map<std::string,std::string>::iterator hostIt = client.data.request.data.headers.find("HOST");
	std::string	domain = (*hostIt).second;
	domain = domain.substr(0, domain.find(':'));

	// what server do you belong to?
	config::ServerBlock	*server = configuration->findServerBlock(domain, client.getPort(), client.getHost());
	if (server == 0) {
		handleError(client, server, 400);
		return;
	}

	// find which route block to use
	utils::Uri uri = client.data.request.data.uri;
	config::RouteBlock	*route = server->findRoute(uri.path);
	if (route == 0) {
		handleError(client, server, 400);
		return;
	}

	// check allowed methods
	if (!route->isAllowedMethod(HTTPParser::methodMap_EtoS.find(client.data.request.data.method)->second)) {
		handleError(client, server, route, 405);
		return;
	}

	if (route->shouldDoFile()) {
		utils::Uri file = route->getRoot();
		file.appendPath(uri.path);
		serveFile(client, *server, *route, file.path);
		return;
	}
	else {
		handleProxy(client, server, route);
		return;
	}
}

void HTTPResponder::handleProxy(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route) {
	globalLogger.logItem(logger::DEBUG, "Handling the proxy connection");

	try {

		client.proxy = new Proxy(route->getProxyUrl().ip, route->getProxyUrl().port);
		client.proxy->createConnection();

		client.addAssociatedFd(client.proxy->getSocket(), associatedFD::WRITE);
		client.responseState = PROXY;
		client.connectionState = ASSOCIATED_FD;

		std::string host = route->getProxyUrl().ip + ":" + utils::intToString(route->getProxyUrl().port);
		std::string x_client = client.getIp();
		std::string x_host = client.data.request.data.headers.find("HOST")->second;
		std::string x_proto = route->getProxyUrl().protocol;

		client.proxy->request.setRequest(
			RequestBuilder(client.data.request.data)
			.setHeader("HOST", host)
			.setHeader("CONNECTION", "Close") // always set so it doesn't hang
			.setHeader("X-FORWARDED-FOR", x_client)
			.setHeader("X-FORWARDED-HOST", x_host)
			.setHeader("X-FORWARDED-PROTO", x_proto)
			.build()
		);

	} catch (Proxy::SocketException &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, server, route, 500);
	} catch (Proxy::ConnectionException &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, server, route, 502);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, server, route, 500);
	}
}

void	HTTPResponder::setEnv(HTTPClient& client, CGIenv::env& envp, std::string& uri, const std::string &f) {
	try {
		CGIenv::ENVBuilder env;
		std::string	domain = (*client.data.request.data.headers.find("HOST")).second;
		domain = domain.substr(0, domain.find(':'));

		env.SERVER_NAME(domain)
			.CONTENT_LENGTH(utils::intToString(client.data.request.data.bodyLength))
			.GATEWAY_INTERFACE("CGI/1.1")
			.PATH_INFO(uri) // TODO URL translating/encoding
			.PATH_TRANSLATED(f)
			.QUERY_STRING(uri.substr(uri.find('?')+1))
			.REMOTE_ADDR(utils::intToString(client.getCliAddr().sin_addr.s_addr)) //TODO convert to valid IP
			.REMOTE_IDENT("") // TODO what is this?
			.REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request.data.method)->second)
			.REQUEST_URI(uri)
			.SCRIPT_NAME(uri)
			.SERVER_PORT(utils::intToString(client.getPort()))
			.SERVER_PROTOCOL("HTTP/1.1")
			.SERVER_SOFTWARE("HTTP 1.1");
			
		std::map<std::string, std::string>::iterator it;
		std::map<std::string, std::string>::iterator end = client.data.request.data.headers.end();
		it = client.data.request.data.headers.find("AUTHORIZATION");
		if (it != end)
			env.AUTH_TYPE(it->second);
		it = client.data.request.data.headers.find("CONTENT_TYPE");
		if (it != end)
			env.CONTENT_TYPE(it->second);
		it = client.data.request.data.headers.find("REMOTE_USER");
		if (it != end)
			env.CONTENT_TYPE(it->second);
			
		envp.setEnv(env.build());
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("ENV could not be built: ") + e.what());
	}
}

// TODO current working directory
// TODO writing body -> wait for primoz
// TODO parsing body
// headers and body
// 2 headers?
// Add unknown headers

void	HTTPResponder::runCGI(HTTPClient& client, const std::string &f, const std::string& cgi) {
	FD				pipefd[2];
	FD				bodyPipefd[2];
	struct stat 	sb;
	CGIenv::env 	envp;
	bool 			body = false;
	client.cgi = new CgiClass;

	setEnv(client, envp, client.data.request.data.uri.path, f);
	char** args = new char *[2]();
	args[0] = utils::strdup(cgi.c_str());

	if (::stat(args[0], &sb) == -1)
		ERROR_THROW(CgiClass::NotFound());

	if (::pipe(pipefd) == -1)
		ERROR_THROW(CgiClass::PipeFail());

	if (client.data.request.data.bodyLength) {
		if (::pipe(bodyPipefd))
			ERROR_THROW(CgiClass::PipeFail());
		body = true;
	}

	int pid = ::fork();
	if (pid == -1)
		ERROR_THROW(CgiClass::ForkFail());
	if (!pid) // TODO ERROR logging
	{
		if (body) {
			if (::dup2(bodyPipefd[0], STDIN_FILENO) == -1) {
				globalLogger.logItem(logger::ERROR, "dup2 failed");
				::exit(1);
			}
			if (::close(bodyPipefd[0]) == -1) {
				globalLogger.logItem(logger::ERROR, "close failed");
				::exit(1);
			}
			if (::close(bodyPipefd[1]) == -1) {
				globalLogger.logItem(logger::ERROR, "close failed");
				::exit(1);
			}
		}
		else {
			if (::close(STDIN_FILENO) == -1) {
				globalLogger.logItem(logger::ERROR, "close failed");
				::exit(1);
			}
		}
		if (::dup2(pipefd[1], STDOUT_FILENO) == -1) {
			globalLogger.logItem(logger::ERROR, "dup2 failed");
			::exit(1);
		}
		if (::close(pipefd[0]) == -1 || ::close(pipefd[1]) == -1) {
			globalLogger.logItem(logger::ERROR, "close failed");
			::exit(1);
		}
		if (::execve(args[0], args, envp.getEnv()) == -1)
			globalLogger.logItem(logger::ERROR, "execve failed");
		::exit(1);
	}
	if (::close(pipefd[1]) == -1)
		ERROR_THROW(CgiClass::CloseFail());
	client.addAssociatedFd(pipefd[0]); //
	if (body) {
		client.addAssociatedFd(bodyPipefd[1], associatedFD::WRITE); //
		if (::close(bodyPipefd[0]) == -1)
			ERROR_THROW(CgiClass::CloseFail());
	}
	client.responseState = CGI; // 
	client.connectionState = ASSOCIATED_FD; //
}
