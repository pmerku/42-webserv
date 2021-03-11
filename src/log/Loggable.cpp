//
// Created by jelle on 3/1/2021.
//

#include "log/Loggable.hpp"

using namespace logger;

ILoggable::ILoggable(): _logger(0) {}

void ILoggable::setLogger(Logger &log) {
	_logger = &log;
}

void ILoggable::logItem(const LogItem &item) const {
	if (_logger) _logger->log(item);
}

void ILoggable::logItem(LogTypes logType, const std::string &msg) const {
	ILoggable::logItem(LogItem(logType, msg));
}

void ILoggable::logItem(const std::string &item) const {
	ILoggable::logItem(LogItem(item));
}

void ILoggable::logItem(const config::ConfigException &e) const {
	if (_logger) _logger->log(LogItem(ERROR, ""), e);
}
