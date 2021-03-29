//
// Created by pmerku on 11/03/2021.
//

#include <stdexcept>
#include "server/http/ResponseBuilder.hpp"
#include "utils/intToString.hpp"
#include "utils/localTime.hpp"
#include "utils/ErrorThrow.hpp"
#include "utils/CreateMap.hpp"
#include "utils/DataList.hpp"

using namespace NotApache;

const std::string ResponseBuilder::_endLine = "\r\n";

const std::map<int, std::string> ResponseBuilder::statusMap =
		utils::CreateMap<int, std::string>
		(100, "Continue")
		(101, "Switching Protocols")
		(102, "Processing")
		(103, "Early hints")
		(200, "OK")
		(201, "Created")
		(202, "Accepted")
		(203, "Non-Authorative information")
		(204, "No Content")
		(205, "Reset Content")
		(206, "Partial Content")
		(207, "Multi-Status")
		(208, "Already Reported")
		(226, "IM Used")
		(300, "Multiple Choices")
		(301, "Moved Permanently")
		(302, "Found")
		(303, "See Other")
		(304, "Not Modified")
		(305, "Use Proxy")
		(306, "Switch Proxy")
		(307, "Temporary Redirect")
		(308, "Permanent Redirect")
		(400, "Bad Request")
		(401, "Unauthorized")
		(402, "Payment Required")
		(403, "Forbidden")
		(404, "Not Found")
		(405, "Method Not Allowed")
		(406, "Not Acceptable")
		(407, "Proxy Authentication Required")
		(408, "Request Timeout")
		(409, "Conflict")
		(410, "Gone")
		(411, "Length Required")
		(412, "Precondition Failed")
		(413, "Payload Too Large")
		(414, "URI Too Long")
		(415, "Unsupported Media Type")
		(416, "Range Not Satisfiable")
		(417, "Expectation Failed")
		(418, "I'm A Teapot")
		(421, "Misdirected Request")
		(422, "Unprocessable Entity")
		(423, "Locked")
		(424, "Failed Dependency")
		(425, "Too Early")
		(426, "Upgrade Required")
		(428, "Precondition Required")
		(429, "Too Many Requests")
		(431, "Request Header Fields Too Large")
		(451, "Unavailable For Legal Reasons")
		(500, "Internal Server Error")
		(501, "Not Implemented")
		(502, "Bad Gateway")
		(503, "Service Unavailable")
		(504, "Gateway Timeout")
		(505, "HTTP Version Not Supported")
		(506, "Variant Also Negotiates")
		(507, "Insufficient Storage")
		(508, "Loop Detected")
		(510, "Not Extended")
		(511, "Network Authentication Required");

ResponseBuilder::ResponseBuilder() {
	_protocol = "HTTP/1.1";
}

ResponseBuilder::ResponseBuilder(const std::string &protocol) {
	_protocol = protocol;
}

ResponseBuilder &ResponseBuilder::setStatus(int code) {
	std::map<int, std::string>::const_iterator it = statusMap.find(code);
	if (it == statusMap.end()) {
		it = statusMap.find(500);
	}
	_statusLine.first = utils::intToString(it->first);
	_statusLine.second = it->second;
	return *this;
}

ResponseBuilder &ResponseBuilder::setHeader(const std::string &key, const std::string &value) {
	_headerMap[key] = value;
	return *this;
}

ResponseBuilder &ResponseBuilder::setBody(const std::string &data, size_t length) {
	setHeader("Content-Length", utils::intToString(length));
	_body.add(data.c_str());
	return *this;
}

ResponseBuilder &ResponseBuilder::setBody(const std::string &data) {
	return setBody(data, data.length());
}

ResponseBuilder &ResponseBuilder::setBody(const utils::DataList &data) {
	setHeader("Content-Length", utils::intToString(data.size()));
	_body = data;
	return *this;
}

ResponseBuilder &ResponseBuilder::setDate() {
	struct timeval tv = {};
	gettimeofday(&tv, NULL);
	setHeader("Date", convertTime(tv.tv_sec));
	return *this;
}

std::string ResponseBuilder::convertTime(time_t time) {
	char date[128];
	tm	*currentTime;

	currentTime = std::localtime(&time); // TODO calculate yourself
	int ret = strftime(date, sizeof(date), "%a, %d %B %Y %H:%M:%S ", currentTime);
	return std::string(date, ret) + "GMT";
}

ResponseBuilder &ResponseBuilder::setServer() {
	setHeader("Server", "Not-Apache");
	return *this;
}

ResponseBuilder &ResponseBuilder::setConnection() {
	setHeader("Connection", "Close");
	return *this;
}

ResponseBuilder &ResponseBuilder::removeHeader(const std::string &header) {
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (it->first == header) {
			_headerMap.erase(it);
			return *this;
		}
	}
	return *this;
}

ResponseBuilder &ResponseBuilder::setDefaults() {
	// if no status code set to default 200
	if (_statusLine.first.empty())
		setStatus(200);

	// if no date header set it
	std::map<std::string, std::string>::iterator it = _headerMap.find("Date");
	if (it == _headerMap.end())
		setDate();

	// if no server header set it
	it = _headerMap.find("Server");
	if (it == _headerMap.end())
		setServer();

	// if no connection header set it
	it = _headerMap.find("Connection");
	if (it == _headerMap.end())
		setConnection();

	// if body is empty set content-length to 0
	if (_body.empty())
		setBody("");

	return *this;
}

utils::DataList	ResponseBuilder::build() {
	// HTTP/1.1 {code} {string value} \r\n
	utils::DataList	output;
	std::string response = _protocol;

	// set defaults
	setDefaults();

	response += " " + _statusLine.first;
	response += " " + _statusLine.second;
	response += _endLine;

	// {Header}: {Header value} \r\n
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (!it->first.empty())
			response += it->first + ": " + it->second;
		response += _endLine;
	}

	// \r\n {body} \r\n
	if (!_body.empty()) {
		response += _endLine;
		output = _body;
		output.add(_endLine.c_str());
	}

	output.add_front(response.c_str());
	return output;
}
