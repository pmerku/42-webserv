//
// Created by jelle on 3/1/2021.
//

#include <ostream>
#include "LogItem.hpp"

using namespace log;

LogItem::LogItem(const std::string &message): msg(message), logType(INFO) {}
LogItem::LogItem(LogTypes logType, const std::string &message): msg(message), logType(logType) {}

std::string LogItem::toString() const {
	return LogItem::toString(0);
}

std::string LogItem::toString(Flags::flagType flags) const {
	std::string str;
	std::string colorPrefix;
	switch (logType) {
		case INFO:
			str = "INFO";
			colorPrefix = "36;1m";
			break;
		case DEBUG:
			str = "DEBUG";
			colorPrefix = "32;1m";
			break;
		case ERROR:
			str = "ERROR";
			colorPrefix = "31;1m";
			break;
		case WARNING:
			str = "WARNING";
			colorPrefix = "33;1m";
			break;
		default:
			str = "UNKNOWN";
			colorPrefix = "37m";
			break;
	}
	if (flags & Flags::Color)
		str = "\u001b[" + colorPrefix + "[" + str + "]" + "\u001b[0m";
	str += " " + msg;
	return str;
}

std::ostream	&operator<<(std::ostream &os, const LogItem &item) {
	return os << item.toString();
}
