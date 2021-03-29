//
// Created by jelle on 3/13/2021.
//

#include "server/terminal/TerminalResponder.hpp"
#include "server/global/GlobalLogger.hpp"

using namespace NotApache;

void TerminalResponder::respond(const std::string &str) {
	// TODO handle terminal command
	(void)str;
	globalLogger.logItem(logger::INFO, "Handled terminal command :)");
}
