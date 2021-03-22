//
// Created by pmerku on 22/03/2021.
//

#include "server/http/RequestBuilder.hpp"
#include "utils/ErrorThrow.hpp"
#include "utils/intToString.hpp"

using namespace NotApache;

RequestBuilder::RequestBuilder() {
	_method = "GET";
}

RequestBuilder::RequestBuilder(const std::string &method) {
	// this could be made a std::vector or std::list to allow more methods
	if (_method != "GET" || _method != "HEAD" || _method != "POST" || _method != "PUT")
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
	_headerMap.insert(std::make_pair(key, value));
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const std::string &data, size_t length) {
	_headerMap.insert(std::make_pair("Content-Length", utils::intToString(length)));
	_body = data;
	return *this;
}

std::string RequestBuilder::endLine() {
	return "\r\n";
}

const std::string &RequestBuilder::build() {
	// {method} {uri} HTTP/1.1 \r\n
	_request = _method;
	_request += " " + _uri;
	_request += " " + _protocol;
	_request += endLine();

	// {Header}: {Header value} \r\n
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (!it->first.empty() && !it->second.empty())
			_request += it->first + ": " + it->second;
		_request += endLine();
	}

	// \r\n {body} \r\n
	if (!_body.empty()) {
		_request += endLine();
		_request += _body;
		_request += endLine();
	}

	return _request;
}
