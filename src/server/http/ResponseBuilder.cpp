//
// Created by pmerku on 11/03/2021.
//

#include <stdexcept>
#include "server/http/ResponseBuilder.hpp"
#include "utils/intToString.hpp"
#include "utils/localTime.hpp"
#include "utils/ErrorThrow.hpp"
#include "utils/CreateMap.hpp"

using namespace NotApache;

// TODO maybe wrap the map into a singleton so clang-tidy doesn't complain with static storage
const std::map<int, std::string> ResponseBuilder::_statusMap =
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
	std::map<int, std::string>::const_iterator it = _statusMap.find(code);
	if (it == _statusMap.end())
		ERROR_THROW(StatusCodeError());
	_statusLine.first = utils::intToString(it->first);
	_statusLine.second = it->second;
	return *this;
}

ResponseBuilder &ResponseBuilder::setHeader(const std::string &key, const std::string &value) {
	_headerMap.insert(std::make_pair(key, value));
	return *this;
}

ResponseBuilder &ResponseBuilder::setBody(const std::string &data, size_t length) {
	_headerMap.insert(std::make_pair("Content-Length", utils::intToString(length)));
	_body = data;
	return *this;
}

ResponseBuilder &ResponseBuilder::setDate() {
	struct timeval tv = {};
	gettimeofday(&tv, NULL);
	_headerMap.insert(std::make_pair("Date", convertTime(tv.tv_sec)));
	return *this;
}

std::string ResponseBuilder::convertTime(time_t time) {
	char date[128];
	tm	*currentTime;

	currentTime = std::localtime(&time); // TODO calculate yourself
	int ret = strftime(date, sizeof(date), "%a, %d %B %Y %H:%M:%S ", currentTime);
	if (ret < 0)
		ERROR_THROW(DateError());

	return std::string(date, ret) + "GMT";
}

std::string ResponseBuilder::endLine() {
	return "\r\n";
}

const std::string &ResponseBuilder::build() {
	// HTTP/1.1 {code} {string value} \r\n
	_response = _protocol;
	_response += " " + _statusLine.first;
	_response += " " + _statusLine.second;
	_response += endLine();

	// {Header}: {Header value} \r\n
	for (std::map<std::string, std::string>::iterator it = _headerMap.begin(); it != _headerMap.end(); it++) {
		if (!it->first.empty() && !it->second.empty())
			_response += it->first + ": " + it->second;
		_response += endLine();
	}

	// \r\n {body} \r\n
	if (!_body.empty()) {
		_response += endLine();
		_response += _body;
		_response += endLine();
	}

	return _response;
}
