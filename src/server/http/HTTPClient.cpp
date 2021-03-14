//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPClient.hpp"

using namespace NotApache;

HTTPClient::HTTPClient(FD clientFd): _fd(clientFd), writeState(NO_RESPONSE), connectionState(READING) {}

FD HTTPClient::getFd() const {
	return _fd;
}
