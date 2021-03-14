//
// Created by jelle on 3/12/2021.
//

#ifndef GLOBALLOGGER_HPP
#define GLOBALLOGGER_HPP

#include "log/Logger.hpp"
#include <iostream>

namespace NotApacheRewrite {
	static logger::Logger globalLogger = logger::Logger(std::cout);
}

#endif //GLOBALLOGGER_HPP
