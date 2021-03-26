//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/http/HTTPParser.hpp"
#include "env/ENVBuilder.hpp"
#include "env/env.hpp"
#include "server/global/GlobalLogger.hpp"
#include "utils/intToString.hpp"

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
			.AUTH_TYPE(client.data.request._headers["AUTHORIZATION"])
			.CONTENT_LENGTH(utils::intToString(client.data.request._body.length()))
			.CONTENT_TYPE(client.data.request._headers["CONTENT_TYPE"])
			.GATEWAY_INTERFACE("CGI/1.1")
			//.PATH_INFO()
			//.PATH_TRANSLATED()
			//.QUERY_STRING()
			//.REMOTE_ADDR()
			//.REMOTE_IDENT()
			.REMOTE_USER(client.data.request._headers["REMOTE-USER"])
			.REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request._method)->second)
			.REQUEST_URI(client.data.request._uri)
			//.SCRIPT_NAME()
			//.SERVER_NAME()
			//.SERVER_PORT()
			.SERVER_PROTOCOL("HTTP/1.1")
			.SERVER_SOFTWARE("HTTP 1.1")
			.build()
		);
		for (int i = 0; envp.getEnv()[i]; i++)
			std::cout << envp.getEnv()[i] << std::endl;
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("ENV could not be built: ") + e.what());
	}
}
