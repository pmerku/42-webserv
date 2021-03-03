//
// Created by jelle on 3/2/2021.
//

#include "server/Client.hpp"

using namespace NotApache;

Client::Client(FD fd, ClientTypes type): _fd(fd), _type(type), _state(READING), _request() {}

FD Client::getFD() const {
	return _fd;
}

void Client::setFD(FD fd) {
	_fd = fd;
}

ClientStates Client::getState() const {
	return _state;
}

void Client::setState(ClientStates state) {
	_state = state;
}

void Client::appendRequest(const std::string &str) {
	_request += str;
}

std::string Client::getRequest() const {
	return _request;
}

void Client::setRequest(const std::string &str) {
	_request = str;
}

ClientTypes Client::getType() const {
	return _type;
}
