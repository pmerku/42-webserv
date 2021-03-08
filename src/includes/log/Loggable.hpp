//
// Created by jelle on 3/1/2021.
//

#ifndef ILOGGABLE_HPP
#define ILOGGABLE_HPP

#include "Logger.hpp"
#include "LogItem.hpp"

namespace log {
	class ILoggable {
	protected:
		Logger *_logger;

	public:
		ILoggable();

		void setLogger(Logger &log);

		void logItem(const LogItem &item);
		void logItem(LogTypes logType, const std::string &msg);
		void logItem(const std::string &msg);
	};
}

#endif
