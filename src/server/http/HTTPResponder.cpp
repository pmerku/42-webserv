//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "utils/intToString.hpp"
#include "utils/strdup.hpp"
#include "utils/ErrorThrow.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/http/HTTPParser.hpp"
#include "env/ENVBuilder.hpp"
#include "env/env.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"
#include "utils/intToString.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

using namespace NotApache;

void HTTPResponder::generateAssociatedResponse(HTTPClient &client) {
	if (client.responseState == FILE) {
		client.data.response.setResponse(
			ResponseBuilder("HTTP/1.1")
			.setStatus(200)
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
	(void)server;
	(void)doErrorPage;
	ResponseBuilder res = ResponseBuilder()
		.setStatus(code)
		.setHeader("Server", "Not-Apache")
		.setDate()
		.setHeader("Connection", "Close");
	// TODO handle error default page
	std::map<int,std::string>::const_iterator it = ResponseBuilder::statusMap.find(code);
	std::string text = it == ResponseBuilder::statusMap.end() ? "Internal server error!" : it->second;
	res.setBody(std::string("<h1>") + utils::intToString(code) + "</h1><p>" + text + "</p>");
	client.data.response.setResponse(res.build());
}

void HTTPResponder::serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &dirPath) {
	// check index
	if (!route.getIndex().empty()) {
		struct stat indexData = {};
		std::string indexFile = dirPath;
		if (0 != indexFile.compare(indexFile.length() - 1, 1, "/"))
			indexFile += "/";
		indexFile += route.getIndex();
		if (::stat(indexFile.c_str(), &indexData) == -1) {
			if (errno != ENOENT) {
				handleError(client, &server, 500);
				return;
			}
		}

		// index file exists, serve it
		// TODO mime types
		if (S_ISREG(indexData.st_mode)) {
			FD fileFd = ::open(indexFile.c_str(), O_RDONLY);
			if (fileFd == -1) {
				handleError(client, &server, 500);
				return;
			}
			client.addAssociatedFd(fileFd);
			client.responseState = FILE;
			client.connectionState = ASSOCIATED_FD;
			return;
		}
	}

	// not index, handle directory listing
	if (route.isDirectoryListing()) {
		DIR *dir = ::opendir(dirPath.c_str());
		if (dir == 0) {
			handleError(client, &server, 500);
			return;
		}
		dirent *dirEntry;
		std::string uriWithoutQuery = client.data.request._uri;
		uriWithoutQuery = uriWithoutQuery.substr(0, uriWithoutQuery.find('?'));
		std::string str = "<h1>";
		str += uriWithoutQuery + "</h1><ul>";
		while ((dirEntry = ::readdir(dir)) != 0) {
			str += "<li><a href=\"";
			str += uriWithoutQuery + "/";
			str += dirEntry->d_name;
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

void HTTPResponder::serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &file) {
	struct stat buf = {};

	// get file data
	if (::stat(file.c_str(), &buf) == -1) {
		if (errno == ENOENT)
			handleError(client, &server, 404);
		else
			handleError(client, &server, 500);
		return;
	}

	// check for directory
	if (S_ISDIR(buf.st_mode)) {
		serveDirectory(client, server, route, file);
		return;
	}
	else if (!S_ISREG(buf.st_mode)) {
		handleError(client, &server, 403);
		return;
	}

	// serve the file
	if (route.shouldDoCgi() && !route.getCgiExt().empty() &&
		0 == file.compare(file.length() - route.getCgiExt().length(), route.getCgiExt().length(), route.getCgiExt())) {
		globalLogger.logItem(logger::DEBUG, "Handling cgi request");
		// TODO handle cgi
		runCGI(client);
	}
	// TODO mime types
	FD fileFd = ::open(file.c_str(), O_RDONLY);
	if (fileFd == -1) {
		handleError(client, &server, 500);
		return;
	}
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
	// TODO check host ip
	config::ServerBlock	*server = configuration->findServerBlock(domain, client.getPort());
	if (server == 0) {
		// TODO what to return?
		handleError(client, server, 400);
		return;
	}

	// find which route block to use
	std::string uriWithoutQuery = client.data.request._uri;
	uriWithoutQuery = uriWithoutQuery.substr(0, uriWithoutQuery.find('?'));
	config::RouteBlock *route = server->findRoute(uriWithoutQuery);
	if (route == 0) {
		handleError(client, server, 400);
		return;
	}
	if (route->shouldDoFile()) {
		std::string file = route->getRoot() + uriWithoutQuery;
		serveFile(client, *server, *route, file);
	}
	else {
		// TODO do proxy
		client.data.response.setResponse(
			ResponseBuilder()
			.setStatus(200)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody("Proxy not yet implemented")
			.build()
		);
	}
}

void	HTTPResponder::setEnv(HTTPClient& client, CGIenv::env& envp, std::string& uri) {
	try {
		char cwd[PATH_MAX];
		if (getcwd(cwd, PATH_MAX) == NULL)
			ERROR_THROW(CWDFail());
		envp.setEnv(CGIenv::ENVBuilder()
			.AUTH_TYPE(client.data.request._headers["AUTHORIZATION"])
			.CONTENT_LENGTH(utils::intToString(client.data.request._body.length()))
			.CONTENT_TYPE(client.data.request._headers["CONTENT_TYPE"])
			.GATEWAY_INTERFACE("CGI/1.1")
			.PATH_INFO(uri) // TODO ?
			.PATH_TRANSLATED(cwd + uri) // TODO ?
			.QUERY_STRING(uri.substr(uri.find('?')+1))
			//.REMOTE_ADDR() ?
			//.REMOTE_IDENT() ?
			.REMOTE_USER(client.data.request._headers["REMOTE_USER"])
			.REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request._method)->second)
			.REQUEST_URI(uri)
			.SCRIPT_NAME(uri)
			//.SERVER_NAME("") host header zonder port
			.SERVER_PORT(utils::intToString(client.getPort()) //port uit port header
			.SERVER_PROTOCOL("HTTP/1.1")
			.SERVER_SOFTWARE("HTTP 1.1")
			.build()
		);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("ENV could not be built: ") + e.what());
	}
}

void	HTTPResponder::runCGI(HTTPClient& client) {
	FD				pipefd[2];
	FD				bodyPipefd[2];
	struct stat 	sb;
	CGIenv::env 	envp;
	bool 			body = false;

	setEnv(client, envp, client.data.request._uri);
	char** args = new char *[2]();
	args[0] = utils::strdup(client.data.request._uri.c_str());

	if (::stat(args[0], &sb) == -1)
		ERROR_THROW(NotFound());

	if (::pipe(pipefd) == -1)
		ERROR_THROW(PipeFail());
	client.data.response._fd = pipefd[0];

	if (client.data.request._body.length()) {
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
	if (!pid)
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
}
