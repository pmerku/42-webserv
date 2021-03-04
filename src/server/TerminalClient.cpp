//
// Created by jelle on 3/4/2021.
//

#include <iostream>

#include "server/TerminalClient.hpp"

using namespace NotApache;

TerminalClient::TerminalClient(FD fd): Client(fd, TERMINAL) {}

// TODO EOF will infinite loop
void TerminalClient::close() {
	const std::string &request = getRequest();
	setRequest(request.substr(request.find('\n')+1));
	setState(READING);
}
