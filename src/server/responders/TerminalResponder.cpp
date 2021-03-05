//
// Created by jelle on 3/4/2021.
//

#include "server/responders/TerminalResponder.hpp"

using namespace NotApache;

TerminalResponder::TerminalResponder(): AResponder("COM") {}

std::string TerminalResponder::generateResponse(Client &client) const {
	(void)client;
	return "Terminal command handled :)\n";
}

std::string TerminalResponder::generateParseError(Client &client) const {
	(void)client;
	return "Couldn't parse this command, this shouldn't happen?\n";
}

std::string TerminalResponder::generateTimeout(Client &client) const {
	(void)client;
	return "Terminal command timed out, this shouldn't happen?\n";
}
