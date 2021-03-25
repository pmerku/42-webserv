//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "env/ENVBuilder.hpp"
#include "env/env.hpp"
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

	try {
		CGIenv::env envp;
		envp.setEnv(CGIenv::ENVBuilder()
				.AUTH_TYPE("authentication")
				.CONTENT_LENGTH("3000")
				.CONTENT_TYPE("text/json")
				.REQUEST_URI("/path")
				.build()
				);
		for (int i = 0; envp.getEnv()[i]; i++)
			std::cout << envp.getEnv()[i] << std::endl;
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("ENV could not be built: ") + e.what());
	}
}
