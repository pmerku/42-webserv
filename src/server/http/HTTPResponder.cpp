//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/global/GlobalLogger.hpp"

using namespace NotApache;

void HTTPResponder::generateResponse(HTTPClient &client) {
	std::string str = "lorem ipsum dolor sit amet";
	try {
		client.data.response.setResponse(
				ResponseBuilder("HTTP/1.1")
				.setStatus(400)
				.setHeader("Server", "Not-Apache")
				.setDate()
			 	.setHeader("Connection", "Close")
			 	.setBody(str, str.length())
			 	.build()
			 	);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("Response could not be built: ") + e.what());
	}
}
