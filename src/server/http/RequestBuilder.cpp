//
// Created by pmerku on 22/03/2021.
//

#include "server/http/RequestBuilder.hpp"
#include "utils/intToString.hpp"
#include "utils/CreateVector.hpp"
#include "server/http/HTTPParser.hpp"
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
	// set defaults
	setDefaults();
}

RequestBuilder::RequestBuilder(const std::string &method) {
	std::vector<std::string>::const_iterator it = std::find(methodArray.begin(), methodArray.end(), method);
	if (it == methodArray.end())
		_method = "GET";
	_method = method;
	// set defaults
	setDefaults();
}

RequestBuilder::RequestBuilder(const HTTPParseData &data) {
	_method = HTTPParser::methodMap_EtoS.find(data.method)->second;
	setURI(data.uri.getFull());
	setProtocol();

	for (std::map<std::string, std::string>::const_iterator it = data.headers.begin(); it != data.headers.end(); ++it) {
		setHeader(it->first, it->second);
	}

	if (data.isChunked)
		setBody(data.chunkedData);
	else
		setBody(data.data);
	// set defaults
	setDefaults();
}

RequestBuilder &RequestBuilder::setURI(const std::string &path) {
	_uri += path;
	return *this;
}

RequestBuilder &RequestBuilder::setHeader(const std::string &key, const std::string &value) {
	utils::toUpper(const_cast<std::string&>(key));
	_headerMap[key] = value;
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const std::string &data, size_t length) {
	setHeader("CONTENT-LENGTH", utils::intToString(length));
	_body.add(data.c_str());
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const utils::DataList &data) {
	setHeader("CONTENT-LENGTH", utils::intToString(data.size()));
	_body = data;
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const std::string &data) {
	return setBody(data, data.length());
}

RequestBuilder &RequestBuilder::setDate() {
	struct timeval tv = {};
	gettimeofday(&tv, NULL);
	setHeader("DATE", convertTime(tv.tv_sec));
	return *this;
}

std::string RequestBuilder::convertTime(time_t time) {
	char date[128];
	tm	*currentTime;

	currentTime = std::localtime(&time);
	int ret = strftime(date, sizeof(date), "%a, %d %B %Y %H:%M:%S ", currentTime);
	return std::string(date, ret) + "GMT";
}

RequestBuilder &RequestBuilder::removeHeader(const std::string &header) {
	utils::toUpper(const_cast<std::string&>(header));
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (it->first == header) {
			_headerMap.erase(it);
			return *this;
		}
	}
	return *this;
}

RequestBuilder &RequestBuilder::setProtocol() {
	_protocol = "HTTP/1.1";
	return *this;
}

RequestBuilder &RequestBuilder::setProtocol(const std::string &protocol) {
	_protocol = protocol;
	return *this;
}

RequestBuilder &RequestBuilder::setDefaults() {
	// if no request URI
	if (_uri.empty())
		setURI("/");

	// if no protocol
	if (_protocol.empty())
		setProtocol();

	// if no date header set it
	std::map<std::string, std::string>::iterator it = _headerMap.find("DATE");
	if (it == _headerMap.end())
		setDate();

	// if body is empty set content-length to 0
	if (_body.empty())
		setHeader("CONTENT-LENGTH", "0");

	return *this;
}

utils::DataList	RequestBuilder::build() {
	utils::DataList output;
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
	request += _endLine;
	if (!_body.empty()) {
		output = _body;
		output.add(_endLine.c_str());
	}

	output.add_front(request.c_str());
	return output;
}
