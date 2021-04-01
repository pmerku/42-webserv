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
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody(client.data.response.getAssociatedDataRaw())
			.build()
		);
		return;
	}
	// TODO proxy & cgi
}

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage) {
	handleError(client, server, 0, code, doErrorPage);
}

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server,  config::RouteBlock *route, int code, bool doErrorPage) {
	// allow header in 405
	if (code == 405) {
		std::string allowedMethods = "";
		for (std::vector<std::string>::const_iterator it = route->getAllowedMethods().begin(); it !=  route->getAllowedMethods().end(); ++it) {
			if (!allowedMethods.empty())
				allowedMethods += ", ";
			allowedMethods += *it;
		}
		client.data.response.builder.setHeader("Allow", allowedMethods);
	}

	// handle error pages
	if (doErrorPage && server != 0 && !server->getErrorPage(code).empty()) {
		struct stat errorPageData = {};
		utils::Uri errorPageFile(server->getErrorPage(code));
		if (::stat(errorPageFile.path.c_str(), &errorPageData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, server, route, 500, false);
				return;
			}
		}
		if (S_ISREG(errorPageData.st_mode)) {
			FD fileFd = ::open(errorPageFile.path.c_str(), O_RDONLY);
			if (fileFd == -1) {
				handleError(client, server, route, 500, false);
				return;
			}
			client.data.response.builder.setHeader("Content-Type", MimeTypes::getMimeType(errorPageFile.getExt()));
			client.data.response.builder.setStatus(code);
			client.addAssociatedFd(fileFd);
			client.responseState = FILE;
			client.connectionState = ASSOCIATED_FD;
			return;
		}
	}

	// generate error page
	client.data.response.builder
		.setStatus(code)
		.setHeader("Server", "Not-Apache")
		.setDate()
		.setHeader("Connection", "Close")
		.setHeader("Content-Type", "text/html");

	std::map<int,std::string>::const_iterator statusIt = ResponseBuilder::statusMap.find(code);
	std::string text = statusIt == ResponseBuilder::statusMap.end() ? "Internal server error!" : statusIt->second;
	client.data.response.builder.setBody(std::string("<h1>") + utils::intToString(code) + "</h1><p>" + text + "</p>");
	if (code == 405) {
		std::string allowedMethods = "";
		for (std::vector<std::string>::const_iterator it = route->getAllowedMethods().begin(); it !=  route->getAllowedMethods().end(); ++it) {
			if (!allowedMethods.empty())
				allowedMethods += ", ";
			allowedMethods += *it;
		}
		client.data.response.builder.setHeader("Allow", allowedMethods);
	}
	client.data.response.setResponse(client.data.response.builder.build());
}

void HTTPResponder::serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &d) {
	// check index
	utils::Uri dirPath = d;
	if (!route.getIndex().empty()) {
		struct stat indexData = {};
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
			FD fileFd = ::open(indexFile.path.c_str(), O_RDONLY);
			if (fileFd == -1) {
				handleError(client, &server, 500);
				return;
			}
			client.data.response.builder.setHeader("Content-Type", MimeTypes::getMimeType(indexFile.getExt()));
			client.data.response.builder.setStatus(200);
			client.addAssociatedFd(fileFd);
			client.responseState = FILE;
			client.connectionState = ASSOCIATED_FD;
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
		utils::Uri uri = client.data.request._uri;
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
		client.data.response.setResponse(
	ResponseBuilder("HTTP/1.1")
			.setStatus(200)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setHeader("Content-Type", "text/html")
			.setBody(str)
			.build()
		);
		return;
	}

	// normal directory handler
	handleError(client, &server, 403);
}

void HTTPResponder::serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f) {
	struct stat buf = {};

	// get file data
	utils::Uri file = f;
	if (::stat(file.path.c_str(), &buf) == -1) {
		if (errno == ENOENT || errno == ENOTDIR)
			handleError(client, &server, 404);
		else
			handleError(client, &server, 500);
		return;
	}

	// check for directory
	if (S_ISDIR(buf.st_mode)) {
		serveDirectory(client, server, route, file.path);
		return;
	}
	else if (!S_ISREG(buf.st_mode)) {
		handleError(client, &server, 403);
		return;
	}

	// serve the file
	if (route.shouldDoCgi() && !route.getCgiExt().empty() && file.getExt() == route.getCgiExt()) {
		globalLogger.logItem(logger::DEBUG, "Handling cgi request");
		// TODO handle cgi
		runCGI(client, f);
		return ;
	}
	FD fileFd = ::open(file.path.c_str(), O_RDONLY);
	if (fileFd == -1) {
		handleError(client, &server, 500);
		return;
	}
	client.data.response.builder.setHeader("Content-Type", MimeTypes::getMimeType(file.getExt()));
	client.data.response.builder.setStatus(200);
	client.addAssociatedFd(fileFd);
	client.responseState = FILE;
	client.connectionState = ASSOCIATED_FD;
}

void HTTPResponder::generateResponse(HTTPClient &client) {
	if (client.data.request._statusCode != 200) {
		// error responses if parsing failed
		handleError(client, 0, client.data.request._statusCode);
		return;
	}
	std::map<std::string,std::string>::iterator hostIt = client.data.request._headers.find("HOST");
	// no host header = invalid request
	if (hostIt == client.data.request._headers.end()) {
		handleError(client, 0, 400);
		return;
	}
	std::string	domain = (*hostIt).second;
	domain = domain.substr(0, domain.find(':'));

	// what server do you belong to?
	config::ServerBlock	*server = configuration->findServerBlock(domain, client.getPort(), client.getHost());
	if (server == 0) {
		handleError(client, server, 400);
		return;
	}

	// find which route block to use
	utils::Uri uri = client.data.request._uri;
	config::RouteBlock	*route = server->findRoute(uri.path);
	if (route == 0) {
		handleError(client, server, 400);
		return;
	}

	// check allowed methods
	if (!route->isAllowedMethod(HTTPParser::methodMap_EtoS.find(client.data.request._method)->second)) {
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
		// TODO do proxy
		route->getProxyUrl();
		client.data.response.setResponse(
			ResponseBuilder()
			.setStatus(200)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody("Proxy not yet implemented")
			.build()
		);
		return;
	}
}

void	HTTPResponder::setEnv(HTTPClient& client, CGIenv::env& envp, std::string& uri, const std::string &f) {
	CGIenv::ENVBuilder env;
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator end = client.data.request._headers.end();
	try {
		char cwd[PATH_MAX];
		if (getcwd(cwd, PATH_MAX) == NULL)
			ERROR_THROW(CWDFail());
		std::string	domain = (*client.data.request._headers.find("HOST")).second;
		domain = domain.substr(0, domain.find(':'));

		env.SERVER_NAME(domain)
			.CONTENT_LENGTH(utils::intToString(client.data.request._body.size()))
			.GATEWAY_INTERFACE("CGI/1.1")
			.PATH_INFO(uri) // TODO URL translating/encoding
			.PATH_TRANSLATED(f)
			.QUERY_STRING(uri.substr(uri.find('?')+1))
			.REMOTE_ADDR(utils::intToString(client.getCliAddr().sin_addr.s_addr)) //TODO convert to valid IP
			.REMOTE_IDENT("") // TODO what is this?
			.REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request._method)->second)
			.REQUEST_URI(uri)
			.SCRIPT_NAME(uri)
			.SERVER_PORT(utils::intToString(client.getPort()))
			.SERVER_PROTOCOL("HTTP/1.1")
			.SERVER_SOFTWARE("HTTP 1.1");
			
		it = client.data.request._headers.find("AUTHORIZATION");
		if (it != end)
			env.AUTH_TYPE(it->second);
		it = client.data.request._headers.find("CONTENT_TYPE");
		if (it != end)
			env.CONTENT_TYPE(it->second);
		it = client.data.request._headers.find("REMOTE_USER");
		if (it != end)
			env.CONTENT_TYPE(it->second);
			
		envp.setEnv(env.build());

		for (int i = 0; envp.getEnv()[i]; i++)
			std::cout << envp.getEnv()[i] << std::endl;
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("ENV could not be built: ") + e.what());
	}
}

void	HTTPResponder::runCGI(HTTPClient& client, const std::string &f) {
	FD				pipefd[2];
	FD				bodyPipefd[2];
	struct stat 	sb;
	CGIenv::env 	envp;
	bool 			body = false;

	setEnv(client, envp, client.data.request._uri, f);
	char** args = new char *[2]();
	//args[0] = utils::strdup(client.data.request._uri.c_str());
	args[0] = utils::strdup("../resources/test-root/env.cgi");

	if (::stat(args[0], &sb) == -1)
		ERROR_THROW(NotFound());

	if (::pipe(pipefd) == -1)
		ERROR_THROW(PipeFail());
	client.data.response._fd = pipefd[0];

	if (client.data.request._body.size()) {
		if (::pipe(bodyPipefd))
			ERROR_THROW(PipeFail());
		client.data.response._bodyfd = bodyPipefd[0];
		if (::close(bodyPipefd[1]) == -1) 
			ERROR_THROW(CloseFail());
		body = true;
	}

	int pid = ::fork();
	if (pid == -1)
		ERROR_THROW(ForkFail());
	if (!pid) // TODO ERROR logging
	{
		if (body) {
			if (::dup2(bodyPipefd[0], STDIN_FILENO) == -1) {
				globalLogger.logItem(logger::ERROR, "dup2 failed");
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
		ERROR_THROW(CloseFail());

	client.addAssociatedFd(pipefd[0]); //
	client.data.response.builder.setHeader("Content-Type", "text/html"); //
	client.data.response.builder.setStatus(200); //
	client.responseState = FILE; // 
	client.connectionState = ASSOCIATED_FD; //
}
