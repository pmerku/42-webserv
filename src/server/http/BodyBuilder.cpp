//
// Created by mramadan on 22/03/2021.
//

#include "server/http/BodyBuilder.hpp"
#include "utils/size_tToString.hpp"

#include <ctime>
#include <cstring>

using namespace NotApache;

const std::string BodyBuilder::_endLine = "\n";
const std::string BodyBuilder::_tab = "\t";


const std::string BodyBuilder::numberFormat(size_t number) {
	return (utils::size_tToString(number));
}

const std::string BodyBuilder::stringFormat(const std::string& str) {
	return (std::string("\"" + str + "\""));
}

const std::string BodyBuilder::typeFormat(const std::string& str) {
	return (stringFormat(str) + ": ");
}

BodyBuilder::BodyBuilder() {
	_body += "{";
	_body += _endLine;
}

BodyBuilder& BodyBuilder::time(const std::string& type, time_t value) {
	char time[128];
	tm	*currentTime;

	currentTime = std::localtime(&value);
	strftime(time, sizeof(time), "%a, %d %B %Y %H:%M:%S", currentTime);

	_body += _tab + typeFormat(type) + stringFormat(time);
	_body += _endLine;
	return *this;
}

BodyBuilder& BodyBuilder::mode(const std::string& type, mode_t value) {
	char mode[128];

	::strmode(value, mode);

	_body += _tab + typeFormat(type) + stringFormat(mode);
	_body += _endLine;
	return *this;
}

BodyBuilder& BodyBuilder::line(const std::string& type, const std::string& value) {
	_body += _tab + typeFormat(type) + stringFormat(value);
	_body += _endLine;
	return *this;
}

BodyBuilder& BodyBuilder::line(const std::string& type, size_t value) {
	_body += _tab + typeFormat(type) + numberFormat(value);
	_body += _endLine;
	return *this;
}

const std::string& BodyBuilder::build() {
	_body += "}";
	_body += _endLine;
	return _body;
}
