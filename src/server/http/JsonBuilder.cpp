//
// Created by mramadan on 22/03/2021.
//

#include "server/http/JsonBuilder.hpp"
#include "utils/size_tToString.hpp"

#include <ctime>
#include <string.h>

using namespace NotApache;

const std::string JsonBuilder::_endLine = "\n";
const std::string JsonBuilder::_tab = "\t";

JsonBuilder::JsonBuilder() {}

const std::string JsonBuilder::numberFormat(size_t number) {
	return (utils::size_tToString(number));
}

const std::string JsonBuilder::stringFormat(const std::string& str) {
	return (std::string("\"" + str + "\""));
}

const std::string JsonBuilder::typeFormat(const std::string& str) {
	return (stringFormat(str) + ": ");
}

JsonBuilder& JsonBuilder::time(const std::string& type, time_t value) {
	char time[128];
	tm	*currentTime;

	currentTime = std::localtime(&value);
	::strftime(time, sizeof(time), "%a, %d %B %Y %H:%M:%S", currentTime);

	addLine(type, time);
	return *this;
}

JsonBuilder& JsonBuilder::mode(const std::string& type, mode_t value) {
	char mode[128];

	::strmode(value, mode); // TODO fix own implementation
	addLine(type, mode);
	return *this;
}

JsonBuilder& JsonBuilder::addLine(const std::string& type, const std::string& value) {
	_bodyMap [typeFormat(type)] = stringFormat(value);
	return *this;
}

JsonBuilder& JsonBuilder::addLine(const std::string& type, size_t value) {
	_bodyMap[typeFormat(type)] = numberFormat(value);
	return *this;
}

const std::string& JsonBuilder::build() {
	_body = "{";
	_body += _endLine;
	std::map<std::string, std::string>::const_iterator it = _bodyMap.begin();
	while (it != _bodyMap.end()) {
		_body += _tab;
		_body += it->first;
		_body += it->second;
		++it;
		if (it != _bodyMap.end())
			_body += ",";
		_body += _endLine;
	}
	_body += "}";
	return _body;
}
