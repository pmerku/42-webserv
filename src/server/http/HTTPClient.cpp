//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPClient.hpp"

using namespace NotApache;

HTTPClient::HTTPClient(FD clientFd, int port): _fd(clientFd), _port(port), writeState(NO_RESPONSE), connectionState(READING), isHandled(false) {}

FD HTTPClient::getFd() const {
	return _fd;
}

int HTTPClient::getPort() const {
	return _port;
}
