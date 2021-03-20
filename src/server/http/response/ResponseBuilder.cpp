//
// Created by pmerku on 11/03/2021.
//

#include <stdexcept>
#include "server/http/response/ResponseBuilder.hpp"
#include "utils/intToString.hpp"
#include "utils/localTime.hpp"

using namespace response;

// TODO maybe wrap the map into a singleton so clang-tidy doesn't complain with static storage
const std::map<int, std::string> ResponseBuilder::_statusMap =
		createMap<int, std::string>
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
		(403, "Forbidden");

ResponseBuilder::ResponseBuilder(const std::string &protocol) {
	this->_response = protocol;
}

ResponseBuilder &ResponseBuilder::setStatus(int code) {
	std::map<int, std::string>::const_iterator it = _statusMap.find(code);
	this->_response += " " + std::string("400"); //utils::intToString(it->first);
	this->_response += " " + it->second;
	return setEndLine();
}

ResponseBuilder &ResponseBuilder::setHeader(const std::string &key, const std::string &value) {
	this->_response += key + ": ";
	this->_response += value;
	return setEndLine();
}

ResponseBuilder &ResponseBuilder::setBody(const std::string &data, size_t length) {
	setContentHeader(length);
	setEndLine();
	this->_response += data;
	return setEndLine();
}

const std::string &ResponseBuilder::build() const {
	return this->_response;
}

ResponseBuilder &ResponseBuilder::setDate() {
	// TODO populate date and time
	this->_response += "Date: ";
	struct timeval tv = {};
	gettimeofday(&tv, NULL);
	return convertTime(tv.tv_sec);
}

ResponseBuilder &ResponseBuilder::setContentHeader(size_t length) {
	(void)length;
	this->_response += "Content-Length: ";
	this->_response += "3000"; //utils::intToString(length);
	return setEndLine();
}

ResponseBuilder &ResponseBuilder::convertTime(time_t currentTime) {
	char date[128];
	struct tm _tm = {};

	int ret = strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S ", utils::relativeLocalTime(&currentTime, &_tm)); // TODO calculate time
	if (ret < 0)
		throw std::runtime_error(strerror(errno)); // TODO exception handling
	this->_response += std::string(date, ret) + "UTC";
	return setEndLine();
}

ResponseBuilder &ResponseBuilder::setEndLine() {
	this->_response += "\r\n";
	return *this;
}
