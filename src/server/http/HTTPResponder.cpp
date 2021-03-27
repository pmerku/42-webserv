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

using namespace NotApache;

void HTTPResponder::generateAssociatedResponse(HTTPClient &client) {
	if (client.responseState == FILE) {
		client.data.response.setResponse(
			ResponseBuilder("HTTP/1.1")
			.setStatus(200)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody(client.data.response.getAssociatedDataRaw(), client.data.response.getAssociatedDataRaw().length())
			.build()
		);
		return;
	}
	// TODO proxy & cgi
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

	// TODO what route block?
	config::RouteBlock	*route = server->getRouteBlocks()[0];
	if (route->shouldDoFile()) {
		struct stat buf = {};
		std::string uriWithoutQuery = client.data.request._uri;
		uriWithoutQuery = uriWithoutQuery.substr(0, host.find('?'));
		std::string file = route->getRoot() + uriWithoutQuery;

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
			if (route->isDirectoryListing()) {
				// TODO list the directory
				std::string str = "directory listing here";
				client.data.response.setResponse(
					ResponseBuilder("HTTP/1.1")
					.setStatus(200)
					.setHeader("Server", "Not-Apache")
					.setDate()
					.setHeader("Connection", "Close")
					.setBody(str, str.length())
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
