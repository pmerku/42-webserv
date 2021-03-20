//
// Created by pmerku on 11/03/2021.
//

#include "server/http/response/AResponse.hpp"
#include "server/http/response/ResponseBuilder.hpp"
#include "server/http/HTTPClient.hpp"
#include "utils/intToString.hpp"

using namespace response;

AResponse::AResponse() { }

AResponse::~AResponse() { }

const std::string &AResponse::getResponse() const {
	return this->_response;
}

void AResponse::setResponse(const NotApache::HTTPClient &client) {
	(void)client;
	std::string str = "lorem ipsum dolor sit amet";
	this->_response =
			ResponseBuilder("HTTP/1.1")
			.setStatus(404)
			.setHeader("Server", "Not-Apache")
			.setDate()
			.setHeader("Connection", "Close")
			.setBody(str, str.length())
			.build();
}
