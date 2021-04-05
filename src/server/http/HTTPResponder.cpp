//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/http/HTTPParser.hpp"
#include "env/ENVBuilder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"
#include "utils/intToString.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <dirent.h>
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
				client.proxy->response.getAssociatedDataRaw()
		);
		return;
	}
	// TODO cgi
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
		.setHeader("Server", "Not-Apache") // TODO remove
		.setDate() // TODO remove
		.setHeader("Connection", "Close") // TODO remove
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
		client.data.response.setResponse(
			ResponseBuilder()
			.setStatus(200) // TODO remove
			.setHeader("Server", "Not-Apache") // TODO remove
			.setDate() // TODO remove
			.setHeader("Connection", "Close") // TODO remove
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
		handleProxy(client, route->getProxyUrl());

		client.proxy->request.setRequest(
			RequestBuilder()
			.setURI("/")
			.setHeader("Host", "designcourse.com")
			.setHeader("Connection", "Close")
			.setHeader("X-Forwarded-For", "192.168.0.1") // TODO get ip address from client
			.setHeader("X-Forwarded-Host", client.data.request.data.headers.find("HOST")->second)
			.setHeader("X-Forwarded-Proto", "http")
			.build()
		);
		return;
	}
}

void HTTPResponder::handleProxy(HTTPClient &client, const std::string &url) {
	globalLogger.logItem(logger::DEBUG, "PROXY handler");
//	std::string::size_type start = url.find("//");
//	std::string::size_type end = url.find('/', start);
//	std::string host = url.substr(start, end);
//	utils::Uri uri(url.substr(end));
	(void)url;
	// parse url
	// get port (if no port, constructor will default to 80)
	client.proxy = new Proxy("66.228.62.75", 80);
//	client.proxy = new Proxy("127.0.0.1", 1337);
//	client.proxy = new Proxy("127.0.0.1", 8081);
	client.proxy->createConnection();

	client.addAssociatedFd(client.proxy->getSocket(), associatedFD::WRITE);
	client.responseState = PROXY;
	client.connectionState = ASSOCIATED_FD;
}
