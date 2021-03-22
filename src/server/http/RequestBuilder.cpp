//
// Created by pmerku on 22/03/2021.
//

#include "server/http/RequestBuilder.hpp"
#include "utils/ErrorThrow.hpp"
#include "utils/intToString.hpp"
#include "utils/CreateVector.hpp"
#include <algorithm>

using namespace NotApache;

const std::string RequestBuilder::_endLine = "\r\n";

const std::vector<std::string> RequestBuilder::methodArray =
		utils::CreateVector<std::string>
			("GET")
			("HEAD")
			("POST")
			("PUT")
			("DELETE")
			("PATCH")
			("OPTIONS")
			("TRACE");

RequestBuilder::RequestBuilder() {
	_method = "GET";
}

RequestBuilder::RequestBuilder(const std::string &method) {
	std::vector<std::string>::const_iterator it = std::find(methodArray.begin(), methodArray.end(), method);
	if (it == methodArray.end())
		ERROR_THROW(MethodError());
	_method = method;
}

RequestBuilder &RequestBuilder::setURI(const std::string &path) {
	if (path.at(0) != '/')
		ERROR_THROW(URIError());
	_uri = path;
	return *this;
}

RequestBuilder &RequestBuilder::setHeader(const std::string &key, const std::string &value) {
	_headerMap[key] = value;
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const std::string &data, size_t length) {
	setHeader("Content-Length", utils::intToString(length));
	_body = data;
	return *this;
}

std::string	RequestBuilder::build() {
	// {method} {uri} HTTP/1.1 \r\n
	std::string request = _method;
	request += " " + _uri;
	request += " " + _protocol;
	request += _endLine;

	// {Header}: {Header value} \r\n
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (!it->first.empty() && !it->second.empty())
			request += it->first + ": " + it->second;
		request += _endLine;
	}

	// \r\n {body} \r\n
	if (!_body.empty()) {
		request += _endLine;
		request += _body;
		request += _endLine;
	}

	return request;
}
