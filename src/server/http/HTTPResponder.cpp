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
#include "server/http/HTTPMimeTypes.hpp"

using namespace NotApache;

// TODO cleanup this file

void HTTPResponder::generateAssociatedResponse(HTTPClient &client) {
	config::ServerBlock *server = configuration->findServerBlock(client.data.request.data.headers.find("HOST")->second, client.getPort(), client.getHost());
	if (client.responseState == FILE) {
		client.data.response.setResponse(
			client.data.response.builder
			.setBody(client.data.response.getAssociatedDataRaw())
			.build()
		);
	} else if (client.responseState == UPLOAD) {
		client.data.response.setResponse(
			client.data.response.builder.build()
		);
	} else if (client.responseState == PROXY) {
		if (client.proxy->response.data.parseStatusCode != 200) {
			handleError(client, server, 502, false);
			return;
		}
		client.data.response.setResponse(
			ResponseBuilder(client.proxy->response.data)
			.build()
		);
	}
	else if (client.responseState == CGI) {
		// parse error
		if (client.cgi->response.data.parseStatusCode != 200) {
			handleError(client, server, 500, false);
			return;
		}

		// child process exit codes
		if (client.cgi->status != 0) {
            if (client.cgi->status == EXECVE_ERROR)
                globalLogger.logItem(logger::ERROR, "CGI error: execve");
            else if (client.cgi->status == CLOSE_ERROR)
                globalLogger.logItem(logger::ERROR, "CGI error: close");
            else if (client.cgi->status == DUP2_ERROR)
                globalLogger.logItem(logger::ERROR, "CGI error: dup2");
            handleError(client, server, 500, false);
			return;
		}

		// cgi success, respond normally
		client.data.response.setResponse(
			ResponseBuilder(client.cgi->response.data)
			.build()
		);
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
			prepareFile(client, *server, *route, errorPageFile, code, false);
			return;
		}
	}

	// generate error page
	client.data.response.builder
		.setStatus(code)
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

	// normal directory handler (also cmon, it should be 403, not 404. stupid 42)
	handleError(client, &server, 404);
}

void HTTPResponder::prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const utils::Uri &file, int code, bool shouldErrorFile) {
	FD fileFd = ::open(file.path.c_str(), O_RDONLY);
	if (fileFd == -1) {
		handleError(client, &server, 500, shouldErrorFile);
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
	client.addAssociatedFd(fileFd);
	client.responseState = NotApache::FILE;
	client.connectionState = ASSOCIATED_FD;
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
		try {
			runCGI(client, file.path, route.getCgi());
		} catch (std::exception &e) {
			globalLogger.logItem(logger::ERROR, std::string("CGI error: ") + e.what());
			handleError(client, &server, &route, 500);
		}
		return;
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

void HTTPResponder::uploadFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f) {
	struct ::stat buf = {};
	std::string message = "Successfully updated file!";

	// get file data
	utils::Uri file = f;
	if (::stat(file.path.c_str(), &buf) == -1) {
		if (errno != ENOENT) {
			if (errno == ENOTDIR)
				handleError(client, &server, &route, 404);
			else
				handleError(client, &server, &route, 500);
			return;
		}
		message = "Successfully created file!";
	}

	// 403 on anything that isnt a regular file
	if (!S_ISREG(buf.st_mode)) {
		handleError(client, &server, 403);
		return;
	}

	// create the file
	FD uploadFd = ::open(file.path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (uploadFd == -1) {
		if (errno == ENOENT)
			handleError(client, &server, 404);
		else
			handleError(client, &server, 500);
		return;
	}

	// setup client for uploading
	client.data.response.builder.setHeader("Content-Type", "text/html");
	client.data.response.builder.setBody(std::string("<h1>") + message + "</h1>");
	client.addAssociatedFd(uploadFd, associatedFD::WRITE);
	client.responseState = NotApache::UPLOAD;
	client.connectionState = ASSOCIATED_FD;
}

void HTTPResponder::deleteFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f) {
	struct ::stat buf = {};

	// get file data
	utils::Uri file = f;
	if (::stat(file.path.c_str(), &buf) == -1) {
		if (errno == ENOENT || errno == ENOTDIR)
			handleError(client, &server, &route, 404);
		else
			handleError(client, &server, &route, 500);
		return;
	}

	// only allow removing of normal files
	if (!S_ISREG(buf.st_mode)) {
		handleError(client, &server, &route, 403);
		return;
	}

	// remove the file
	int result = ::unlink(file.path.c_str());
	if (result == -1) {
		handleError(client, &server, &route, 500);
		return;
	}

	client.data.response.setResponse(
		ResponseBuilder()
		.setHeader("Content-Type", "text/html")
		.setBody("<h1>Successfully deleted file!</h1>")
		.build()
	);
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
	std::string rewrittenUrl = uri.path; // findRoute will rewrite url
	config::RouteBlock	*route = server->findRoute(rewrittenUrl);
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
		file.appendPath(rewrittenUrl);

		// use upload directory instead for upload modifications
		if (client.data.request.data.method == DELETE || client.data.request.data.method == PUT)
			file = utils::Uri(route->getSaveUploads());
		file.appendPath(uri.path);
		if (client.data.request.data.method == DELETE)
			deleteFile(client, *server, *route, file.path);
		else if (client.data.request.data.method == PUT)
			uploadFile(client, *server, *route, file.path);
		else
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

// TODO current working directory
// TODO php not working
void	HTTPResponder::runCGI(HTTPClient& client, const std::string &filePath, const std::string& cgiPath) {
	FD				pipefd[2];
	FD				bodyPipefd[2];
	struct stat 	sb;
	bool 			body = false;
	client.cgi = new CgiClass;

	client.cgi->generateENV(client, client.data.request.data.uri, filePath, cgiPath);
	char** args = new char *[3]();
	args[0] = utils::strdup(cgiPath);
	args[1] = utils::strdup(filePath);

	if (::stat(args[0], &sb) == -1)
		ERROR_THROW(CgiClass::NotFound());

	if (::pipe(pipefd) == -1)
		ERROR_THROW(CgiClass::PipeFail());
    if (::pipe(bodyPipefd))
        ERROR_THROW(CgiClass::PipeFail());

	if (client.data.request.data.bodyLength)
		body = true;

    client.cgi->pid = ::fork();
	client.cgi->hasExited = false;
	if (client.cgi->pid == -1)
		ERROR_THROW(CgiClass::ForkFail());
	if (!client.cgi->pid) {
		// set body pipes
        if (::dup2(bodyPipefd[0], STDIN_FILENO) == -1)
            ::exit(DUP2_ERROR);
        if (::close(bodyPipefd[0]) == -1)
            ::exit(CLOSE_ERROR);
        if (::close(bodyPipefd[1]) == -1)
            ::exit(CLOSE_ERROR);

		// set output pipes
		if (::dup2(pipefd[1], STDOUT_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(pipefd[0]) == -1 || ::close(pipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// run cgi
		::execve(args[0], args, client.cgi->getEnvp().getEnv());
        ::exit(EXECVE_ERROR);
	}
	if (::close(pipefd[1]) == -1)
		ERROR_THROW(CgiClass::CloseFail());
    if (::close(bodyPipefd[0]) == -1)
        ERROR_THROW(CgiClass::CloseFail());

	client.addAssociatedFd(pipefd[0]);
	if (body)
		client.addAssociatedFd(bodyPipefd[1], associatedFD::WRITE);
	else if (::close(bodyPipefd[1]) == -1)
        ERROR_THROW(CgiClass::CloseFail());

	client.responseState = CGI;
	client.connectionState = ASSOCIATED_FD;
}
