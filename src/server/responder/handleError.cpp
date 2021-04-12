//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "utils/intToString.hpp"
#include <cerrno>

using namespace NotApache;

void HTTPResponder::handleError(HTTPClient &client, int code) {
	// Request authentication
	if (code == 401 && client.routeBlock)
		client.data.response.builder.setHeader("WWW-AUTHENTICATE", "Basic realm=\"Not-Apache\"");

	// allow header in 405
	if (code == 405 && client.routeBlock)
		client.data.response.builder.setAllowedMethods(client.routeBlock->getAllowedMethods());

	// handle error pages
	if (!client.hasErrored && client.serverBlock != 0 && !client.serverBlock->getErrorPage(code).empty()) {
		client.hasErrored = true; // to prevent looping of error pages
		struct ::stat errorPageData = {};
		utils::Uri errorPageFile(client.serverBlock->getErrorPage(code));
		if (::stat(errorPageFile.path.c_str(), &errorPageData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, 500);
				return;
			}
		}
		if (S_ISREG(errorPageData.st_mode)) {
			client.file = errorPageFile;
			prepareFile(client, code);
			return;
		}
	}

	// generate error page
	client.data.response.builder
			.setStatus(code)
			.setHeader("Content-Type", "text/html");

	std::map<int,std::string>::const_iterator statusIt = ResponseBuilder::statusMap.find(code);
	std::string text = statusIt == ResponseBuilder::statusMap.end() ? "Internal server error!" : statusIt->second;
	client.replyStatus = code;
	client.data.response.builder.setBody(std::string("<h1>") + utils::intToString(code) + "</h1><p>" + text + "</p>");
	client.data.response.setResponse(client.data.response.builder.build());
}
