//
// Created by jelle on 3/1/2021.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <vector>
#include <ostream>
#include "LogItem.hpp"

namespace logger {

	class Logger {
	private:
		std::vector<std::ostream *> _streams;
		Flags::flagType 			_loggerFlags;

	public:
		Logger(std::ostream &stream);
		Logger(const std::vector<std::ostream *> &streams);
		Logger(const Logger &logger);

		void 	setFlags(Flags::flagType flags);

		void	log(const LogItem &item);

		Logger &operator=(const Logger &rhs);
	};

}

#endif
