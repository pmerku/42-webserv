//
// Created by jelle on 3/12/2021.
//

#include "TCPListener.hpp"
#include <unistd.h>
#include <fcntl.h>

using namespace NotApacheRewrite;

TCPListener::TCPListener(int port, int backLog): _port(port), _fd(-1), _backlog(backLog) {}

TCPListener::~TCPListener() {
	if (_fd > 0) ::close(_fd);
}

FD TCPListener::getFD() {
	return _fd;
}

void TCPListener::start() {
	int one = 1;

	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		throw FailedToListen();
	}

	// set socket options
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons(_port);

	if (::bind(_fd, reinterpret_cast<struct sockaddr *>(&svr_addr), sizeof(svr_addr)) == -1) {
		::close(_fd);
		_fd = -1;
		throw FailedToListen();
	}

	if (::listen(_fd, _backlog) == -1) {
		::close(_fd);
		_fd = -1;
		throw FailedToListen();
	}
}

HTTPClient *TCPListener::acceptClient() {
	sockaddr_in	cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int client_fd = accept(_fd, (struct sockaddr *) &cli_addr, &sin_len);
	if (client_fd == -1) {
		throw FailedToAccept();
	}

	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	HTTPClient	*out = new HTTPClient(client_fd);
	return out;
}
