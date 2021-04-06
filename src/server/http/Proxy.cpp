//
// Created by pmerku on 30/03/2021.
//

#include "server/http/Proxy.hpp"
#include "utils/ErrorThrow.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace NotApache;

Proxy::Proxy(const std::string &url, int port) : _url(url), _port(port), response(HTTPParseData::RESPONSE) {
	_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		ERROR_THROW(SocketException());
	::fcntl(_socket, O_NONBLOCK);
}

Proxy::~Proxy() {
	::close(_socket);
}

void Proxy::createConnection() const {
	struct sockaddr_in _proxyAddress = {};
	_proxyAddress.sin_family = AF_INET;
	_proxyAddress.sin_addr.s_addr = ::inet_addr(_url.c_str());
	_proxyAddress.sin_port = htons(_port);

	if (::connect(_socket, reinterpret_cast<struct sockaddr*>(&_proxyAddress), sizeof(_proxyAddress)) < 0)
		ERROR_THROW(ConnectionException());
}

FD Proxy::getSocket() const {
	return _socket;
}

void Proxy::setURI(const utils::Uri &uri) {
	_uri = uri;
}

utils::Uri Proxy::getURI() const {
	return _uri;
}
