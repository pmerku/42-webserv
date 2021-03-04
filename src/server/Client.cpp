//
// Created by jelle on 3/2/2021.
//

#include "server/Client.hpp"
#include <unistd.h>

using namespace NotApache;

Client::Client(FD fd, ClientTypes type): _fd(fd), _type(type), _state(READING), _dataType(), _request(), _response() {}

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

std::string Client::getDataType() const {
	return _dataType;
}

std::string Client::getResponse() const {
	return _response;
}

std::size_t Client::getResponseIndex() const {
	return _responseIndex;
}

void Client::setDataType(const std::string &str) {
	_dataType = str;
}

void Client::appendResponse(const std::string &str) {
	_response += str;
}

void Client::setResponse(const std::string &str) {
	_response = str;
}

void Client::setResponseIndex(std::size_t i) {
	_responseIndex = i;
}

ResponseStates Client::getResponseState() const {
	return _responseState;
}

void Client::setResponseState(ResponseStates state) {
	_responseState = state;
}

void Client::close() {
	::close(_fd);
	_state = CLOSED;
}

Client::~Client() {}
