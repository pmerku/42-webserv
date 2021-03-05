//
// Created by jelle on 3/3/2021.
//

#include "server/listeners/TerminalListener.hpp"
#include <fcntl.h>

using namespace NotApache;

TerminalListener::TerminalListener(): _client(new TerminalClient(0, 1)) {
	fcntl(_client->getReadFD(), F_SETFL, O_NONBLOCK);
}

TerminalListener::~TerminalListener() {
	delete _client;
}

FD TerminalListener::getFD() {
	return 0;
}

void TerminalListener::start() {}

Client *TerminalListener::acceptClient() {
	return _client;
}
