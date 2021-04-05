//
// Created by pmerku on 22/03/2021.
//

#include "server/http/RequestBuilder.hpp"
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
	_protocol = "HTTP/1.1";
}

RequestBuilder::RequestBuilder(const std::string &method) {
	std::vector<std::string>::const_iterator it = std::find(methodArray.begin(), methodArray.end(), method);
	if (it == methodArray.end())
		_method = "GET";
	_method = method;
}

RequestBuilder &RequestBuilder::setURI(const std::string &path) {
	if (path.at(0) != '/')
		_uri = "/";
	_uri += path;
	return *this;
}

RequestBuilder &RequestBuilder::setHeader(const std::string &key, const std::string &value) {
	_headerMap[key] = value;
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const std::string &data, size_t length) {
	setHeader("Content-Length", utils::intToString(length));
	_body.add(data.c_str());
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const utils::DataList &data) {
	setHeader("Content-Length", utils::intToString(data.size()));
	_body = data;
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const std::string &data) {
	return setBody(data, data.length());
}

RequestBuilder &RequestBuilder::setDate() {
	struct timeval tv = {};
	gettimeofday(&tv, NULL);
	setHeader("Date", convertTime(tv.tv_sec));
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
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (it->first == header) {
			_headerMap.erase(it);
			return *this;
		}
	}
	return *this;
}

RequestBuilder &RequestBuilder::setDefaults() {
	// if no date header set it
	std::map<std::string, std::string>::iterator it = _headerMap.find("Date");
	if (it == _headerMap.end())
		setDate();

	// if body is empty set content-length to 0
	if (_body.empty())
		setBody("");

	return *this;
}

utils::DataList	RequestBuilder::build() {
	utils::DataList output;
	// {method} {uri} HTTP/1.1 \r\n
	std::string request = _method;

	// set defaults
	setDefaults();

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
