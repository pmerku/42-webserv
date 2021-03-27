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

void HTTPResponder::serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &file) {
	(void)server;
	struct stat buf = {};

	if (::stat(file.c_str(), &buf) == -1) {
		if (errno == ENOENT) {
			std::string str = "File not found";
			client.data.response.setResponse(
					ResponseBuilder("HTTP/1.1")
							.setStatus(404)
							.setHeader("Server", "Not-Apache")
							.setDate()
							.setHeader("Connection", "Close")
							.setBody(str, str.length())
							.build()
			);
			return;
		}
		std::string str = "Something went wrong";
		client.data.response.setResponse(
				ResponseBuilder("HTTP/1.1")
						.setStatus(500)
						.setHeader("Server", "Not-Apache")
						.setDate()
						.setHeader("Connection", "Close")
						.setBody(str, str.length())
						.build()
		);
		return;
	}

	if (S_ISDIR(buf.st_mode)) {
		// check index
		if (!route.getIndex().empty()) {
			struct stat indexData = {};
			std::string indexFile = file;
			if (0 != indexFile.compare(indexFile.length() - 1, 1, "/"))
				indexFile += "/";
			indexFile += route.getIndex();
			if (::stat(indexFile.c_str(), &indexData) == -1) {
				if (errno != ENOENT) {
					std::string str = "Something went wrong";
					client.data.response.setResponse(
							ResponseBuilder("HTTP/1.1")
									.setStatus(500)
									.setHeader("Server", "Not-Apache")
									.setDate()
									.setHeader("Connection", "Close")
									.setBody(str, str.length())
									.build()
					);
				}
			}
			if (S_ISREG(indexData.st_mode)) {
				// index file exists, serve it
				FD fileFd = ::open(indexFile.c_str(), O_RDONLY);
				if (fileFd == -1) {
					std::string str = "Something went wrong";
					client.data.response.setResponse(
							ResponseBuilder("HTTP/1.1")
									.setStatus(500)
									.setHeader("Server", "Not-Apache")
									.setDate()
									.setHeader("Connection", "Close")
									.setBody(str, str.length())
									.build()
					);
					return;
				}
				client.addAssociatedFd(fileFd);
				client.responseState = FILE;
				client.connectionState = ASSOCIATED_FD;
				return;
			}
		}

		// not index, handle normal directory
		if (route.isDirectoryListing()) {
			DIR *dir = ::opendir(file.c_str());
			if (dir == 0) {
				client.data.response.setResponse(
					ResponseBuilder()
					.setStatus(500)
					.setHeader("Server", "Not-Apache")
					.setDate()
					.setHeader("Connection", "Close")
					.setBody("Something went wrong")
					.build()
				);
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
		std::string str = "File not found";
		client.data.response.setResponse(
				ResponseBuilder("HTTP/1.1")
						.setStatus(404)
						.setHeader("Server", "Not-Apache")
						.setDate()
						.setHeader("Connection", "Close")
						.setBody(str, str.length())
						.build()
		);
		return;
	}
	else if (!S_ISREG(buf.st_mode)) {
		std::string str = "File not found";
		client.data.response.setResponse(
				ResponseBuilder("HTTP/1.1")
						.setStatus(404)
						.setHeader("Server", "Not-Apache")
						.setDate()
						.setHeader("Connection", "Close")
						.setBody(str, str.length())
						.build()
		);
		return;
	}

	// serve the file
	// TODO check file extension for cgi
	FD fileFd = ::open(file.c_str(), O_RDONLY);
	if (fileFd == -1) {
		std::string str = "Something went wrong";
		client.data.response.setResponse(
				ResponseBuilder("HTTP/1.1")
						.setStatus(500)
						.setHeader("Server", "Not-Apache")
						.setDate()
						.setHeader("Connection", "Close")
						.setBody(str, str.length())
						.build()
		);
		return;
	}
	client.addAssociatedFd(fileFd);
	client.responseState = FILE;
	client.connectionState = ASSOCIATED_FD;
	return;
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
	if (hostIt == client.data.request._headers.end()) {
		// TODO what if no host header?
		std::string str = "Missing host header";
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
	std::string	host = (*hostIt).second;
	host = host.substr(0, host.find(':'));

	// what server do you belong to?
	// TODO check host ip
	config::ServerBlock	*server = configuration->findServerBlock(host, client.getPort());
	if (server == 0) {
		std::string str = "No server block to handle";
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

	// find which route block to use
	std::string uriWithoutQuery = client.data.request._uri;
	uriWithoutQuery = uriWithoutQuery.substr(0, uriWithoutQuery.find('?'));
	config::RouteBlock	*route = server->findRoute(uriWithoutQuery);
	if (route == 0) {
		client.data.response.setResponse(
			ResponseBuilder()
			.setStatus(404)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody("Route not found")
			.build()
		);
		return;
	}
	if (route->shouldDoFile()) {
		std::string file = route->getRoot() + uriWithoutQuery;
		HTTPResponder::serveFile(client, *server, *route, file);
	}
	else {
		// TODO do proxy
		std::string str = "Proxy not yet implemented";
		client.data.response.setResponse(
			ResponseBuilder("HTTP/1.1")
			.setStatus(200)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody(str, str.length())
			.build()
		);
	}
}
