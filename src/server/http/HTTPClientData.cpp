//
// Created by jelle on 3/14/2021.
//

#include "server/http/HTTPClientData.hpp"
#include <iostream>

namespace NotApache
{
	std::string methodAsString(const e_method& in) {
		switch (in) {
			case GET:
				return "GET";
			case HEAD:
				return "HEAD";
			case POST:
				return "POST";
			case PUT:
				return "PUT";
			case DELETE:
				return "DELETE";
			case CONNECT:
				return "CONNECT";
			case OPTIONS:
				return "OPTIONS";
			case TRACE:
				return "TRACE";
			default:
				return "INVALID METHOD";
		}
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& x) {
		for (size_t i = 0; i < x.size(); ++i)
			o << x[i] << std::endl;
		return o;
	}

	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x) {
		o	<< "==REQUEST=="								<< std::endl
			<< "Method: "	<< methodAsString(x._method)	<< std::endl
			<< "URI: "		<< x._uri						<< std::endl;
			if (x._headers.size()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<std::string, std::string>::iterator it = x._headers.begin(); it != x._headers.end(); ++it)
					std::cout << "Header: [" << it->first << ": " << it->second << "]" << std::endl;
			}
			else
				std::cout << "-NO HEADERS-" << std::endl;
			if (x._bodySize)
				std::cout << "Body size: " << x._bodySize << std::endl;
			if (x._body.length()) {
				std::cout << "Body length: " << x._body.length() << std::endl;
				std::cout << std::endl << "-BODY-" << std::endl << x._body << std::endl;
			}
			else
				std::cout << std::endl << "-NO BODY-" << std::endl;
		return o;
	}
}

using namespace NotApache;

HTTPClientRequest::HTTPClientRequest(): _rawRequest(), _method(), _uri(), _headers(), _body(), _bodySize(), _methodMap(), _statusCode(), _isChunked(false) {
	_methodMap["GET"] = GET; // in static iets
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
	_methodMap["DELETE"] = DELETE;
	_methodMap["CONNECT"] = CONNECT;
	_methodMap["OPTIONS"] = OPTIONS;
	_methodMap["TRACE"] = TRACE;
}

const std::string &HTTPClientRequest::getRawRequest() const {
	return _rawRequest;
}

void HTTPClientRequest::appendRequestData(const std::string &newData) {
	_rawRequest += newData;
}

void HTTPClientRequest::setRawRequest(const std::string &newData) {
	_rawRequest = newData;
}

HTTPClientResponse::HTTPClientResponse(): _response(),  _progress(0) {}

const std::string &HTTPClientResponse::getResponse() const {
	return _response;
}

std::string::size_type HTTPClientResponse::getProgress() const {
	return _progress;
}

void HTTPClientResponse::setResponse(const std::string &response) {
	_response = response;
}

void HTTPClientResponse::setProgress(std::string::size_type index) {
	_progress = index;
}

