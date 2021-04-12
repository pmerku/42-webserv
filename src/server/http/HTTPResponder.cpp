//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalConfig.hpp"

using namespace NotApache;

void HTTPResponder::generateResponse(HTTPClient &client) {
	// error responses if parsing failed
	if (client.data.request.data.parseStatusCode != 200) {
		handleError(client, client.data.request.data.parseStatusCode);
		return;
	}

	// get domain
	std::map<std::string,std::string>::iterator hostIt = client.data.request.data.headers.find("HOST");
	std::string	domain = (*hostIt).second;
	domain = domain.substr(0, domain.find(':'));

	// what server do you belong to?
	client.serverBlock = configuration->findServerBlock(domain, client.getPort(), client.getHost());
	if (client.serverBlock == 0) {
		handleError(client, 400);
		return;
	}

	// respond to * OPTIONS request
	if (client.data.request.data.uri.isWildcard) {
		client.data.response.setResponse(ResponseBuilder().build());
		return;
	}

	// find which route block to use
	client.rewrittenUrl = client.data.request.data.uri.path; // findRoute will rewrite url
	client.routeBlock = client.serverBlock->findRoute(client.rewrittenUrl );
	if (client.routeBlock == 0) {
		handleError(client, 400);
		return;
	}

	// check allowed methods
	if (!client.routeBlock->isAllowedMethod(HTTPParser::methodMap_EtoS.find(client.data.request.data.method)->second)) {
		handleError(client, 405);
		return;
	}

	// check authorisation
	if (!client.routeBlock->getAuthBasic().empty()) {
		std::map<std::string, std::string>::iterator it = client.data.request.data.headers.find("AUTHORIZATION");
		if (it == client.data.request.data.headers.end()) {
			handleError(client, 401);
			return ;
		}
		else {
			try {
				if (!checkCredentials(client.routeBlock->getAuthorized(), it->second)) {
					handleError(client, 403);
					return;
				}
			}
			catch (const std::exception& e) {
				globalLogger.logItem(logger::ERROR, std::string("File serving error: ") + e.what());
				handleError(client, 500);
				return;
			}
		}
	}

	// normal HTTP handling
	if (client.routeBlock->shouldDoFile()) {
		client.file = client.routeBlock->getRoot();

		// use upload directory instead for upload modifications
		if (client.data.request.data.method == DELETE || client.data.request.data.method == PUT)
			client.file = utils::Uri(client.routeBlock->getSaveUploads());
		client.file.appendPath(client.rewrittenUrl);

		// choose action
		if (client.data.request.data.method == DELETE)
			deleteFile(client);
		else if (client.data.request.data.method == PUT)
			uploadFile(client);
		else
			serveFile(client);
		return;
	}
	// do proxy
	else {
		handleProxy(client);
		return;
	}
}
