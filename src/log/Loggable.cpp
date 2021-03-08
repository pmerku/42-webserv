//
// Created by jelle on 3/1/2021.
//

#include "log/Loggable.hpp"

using namespace log;

ILoggable::ILoggable(): _logger(0) {}

void ILoggable::setLogger(Logger &log) {
	_logger = &log;
}

void ILoggable::logItem(const LogItem &item) {
	if (_logger) _logger->log(item);
}

void ILoggable::logItem(LogTypes logType, const std::string &msg) {
	ILoggable::logItem(LogItem(logType, msg));
}

void ILoggable::logItem(const std::string &item) {
	ILoggable::logItem(LogItem(item));
}
