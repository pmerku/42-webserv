//
// Created by jelle on 3/13/2021.
//

#include "TerminalResponder.hpp"
#include "rewrite-server/global/GlobalLogger.hpp"

using namespace NotApacheRewrite;

void TerminalResponder::respond(const std::string &str) {
	(void)str;
	globalLogger.log(logger::LogItem(logger::INFO, "Handled terminal command :)"));
}
