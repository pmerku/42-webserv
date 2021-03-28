//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
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
	std::string text;
	ResponseBuilder res = ResponseBuilder()
		.setStatus(code)
		.setHeader("Server", "Not-Apache")
		.setDate()
		.setHeader("Connection", "Close");
	// TODO handle error default page
	switch (code) {
		case 404:
			text = "File not found!";
			break;
		case 403:
			text = "Forbidden!";
			break;
		default:
		case 500:
			text = "Internal server error!";
			break;
		case 400:
			text = "Bad request!";
			break;
	}
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

	// TODO generate error responses
	if (false) {
		// error responses if parsing failed
		std::string str = "Couldn't parse the HTTP request";
		client.data.response.setResponse(
			ResponseBuilder("HTTP/1.1")
			.setStatus(400)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody(str, str.length())
			.build()
		);
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
	config::RouteBlock	*route = server->findRoute(uriWithoutQuery);
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
