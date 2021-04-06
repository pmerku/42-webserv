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

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route, int code, bool doErrorPage) {
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
		.setHeader("Server", "Not-Apache")
		.setDate()
		.setHeader("Connection", "Close")
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
		// TODO handle cgi
	}
	prepareFile(client, server, route, buf, file);
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
