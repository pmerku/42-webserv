//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPClient.hpp"
#include <unistd.h>
#include <sys/socket.h>

using namespace NotApache;

HTTPClient::HTTPClient(FD clientFd, int port, long host, sockaddr_in cli_addr) : _fd(clientFd), _port(port), _host(host), _cli_addr(cli_addr), _associatedFds(), writeState(NO_RESPONSE), connectionState(READING), responseState(NONE), isHandled(false) {}

HTTPClient::~HTTPClient() {
	for (std::vector<FD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); it++) {
		::close(*it);
	}
	_associatedFds.clear();
}

FD HTTPClient::getFd() const {
	return _fd;
}

int HTTPClient::getPort() const {
	return _port;
}

long HTTPClient::getHost() const {
	return _host;
}

sockaddr_in HTTPClient::getCliAddr() const {
	return _cli_addr;
}

void HTTPClient::addAssociatedFd(FD fd) {
	for (std::vector<FD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); it++) {
		if (*it == fd)
			return;
	}
	_associatedFds.push_back(fd);
}

void HTTPClient::removeAssociatedFd(FD fd) {
	for (std::vector<FD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); it++) {
		if (*it == fd) {
			::close(*it);
			_associatedFds.erase(it);
			return;
		}
	}
}

FD HTTPClient::getAssociatedFd(std::vector<FD>::size_type i) const {
	return _associatedFds[i];
}

std::vector<FD>::size_type HTTPClient::getAssociatedFdLength() const {
	return _associatedFds.size();
}
