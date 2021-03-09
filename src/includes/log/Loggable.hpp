//
// Created by jelle on 3/1/2021.
//

#ifndef ILOGGABLE_HPP
#define ILOGGABLE_HPP

#include "Logger.hpp"
#include "LogItem.hpp"
#include "config/ConfigException.hpp"

namespace logger {
	class ILoggable {
	protected:
		Logger *_logger;

	public:
		ILoggable();

		void setLogger(Logger &log);

		void logItem(const config::ConfigException &e) const;
		void logItem(const LogItem &item) const;
		void logItem(const std::string &msg) const;
		void logItem(LogTypes logType, const std::string &msg) const;
	};
}

#endif
