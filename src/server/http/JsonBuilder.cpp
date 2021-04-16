//
// Created by mramadan on 22/03/2021.
//

#include "server/http/JsonBuilder.hpp"
#include "utils/size_tToString.hpp"
#include <sys/stat.h>

#include <ctime>

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

char JsonBuilder::getMode(mode_t value, int mask, char c) {
	if (value & mask)
		return (c);
	else
		return ('-');
}

JsonBuilder& JsonBuilder::mode(const std::string& type, mode_t value) {
	std::string mode;

	// is directory or file
	if (S_ISDIR(value))
		mode = 'd';
	else
		mode = '-';

	// Get user permissions
	mode += getMode(value, S_IRUSR, 'r');
	mode += getMode(value, S_IWUSR, 'w');
	mode += getMode(value, S_IXUSR, 'x');

	// Get group permissions
	mode += getMode(value, S_IRGRP, 'r');
	mode += getMode(value, S_IWGRP, 'w');
	mode += getMode(value, S_IXGRP, 'x');

	// Get other permissions
	mode += getMode(value, S_IROTH, 'r');
	mode += getMode(value, S_IWOTH, 'w');
	mode += getMode(value, S_IXOTH, 'x');
	
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
