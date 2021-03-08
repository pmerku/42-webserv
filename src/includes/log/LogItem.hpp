//
// Created by jelle on 3/1/2021.
//

#ifndef LOGITEM_HPP
#define LOGITEM_HPP

#include <string>

namespace logger {

	enum LogTypes {
		INFO,
		ERROR,
		WARNING,
		DEBUG
	};

	class Flags {
	public:
		typedef int flagType;
		static const flagType 	Color = 1; // 0b001
		static const flagType 	Debug = 2; // 0b010
	};

	class LogItem {
	public:
		std::string msg;
		LogTypes logType;

		LogItem(const std::string &msg);
		LogItem(LogTypes logType, const std::string &msg);

		std::string	toString() const;
		std::string	toString(Flags::flagType flags) const;
	};

}

std::ostream	&operator<<(std::ostream &os, const logger::LogItem &item);

#endif
