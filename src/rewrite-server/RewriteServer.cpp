//
// Created by jelle on 3/12/2021.
//

#include "RewriteServer.hpp"

using namespace NotApacheRewrite;

RewriteServer::SelectReturn RewriteServer::_runSelect() {
	// TODO oob tcp data?
	// TODO do timeouts
	int ret = ::select(_maxFd + 1, &_readFdSet, &_writeFdSet, 0, 0);
	if (ret == 0)
		return TIMEOUT;
	else if (ret == -1)
		return ERROR;
	return SUCCESS;
}

void RewriteServer::_createFdSets() {
	// clear sets
	_maxFd = 0;
	FD_ZERO(&_writeFdSet);
	FD_ZERO(&_readFdSet);

	// add listeners (sockets)
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i) {
		FD	fd = (*i)->getFD();
		if (fd > _maxFd) _maxFd = fd;
		FD_SET(fd, &_readFdSet);
	}

	// add event bus & term client
	if (_eventBus.getReadFD() > _maxFd) _maxFd = _eventBus.getReadFD();
	FD_SET(_eventBus.getReadFD(), &_readFdSet);
	if (_termClient.getFd() > _maxFd) _maxFd = _termClient.getFd();
	FD_SET(_termClient.getFd(), &_readFdSet);

	// add clients
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		if ((*i)->connectionState == READING || (*i)->connectionState == WRITING) {
			FD	fd = (*i)->getFd();
			if (fd > _maxFd) _maxFd = fd;
			if ((*i)->connectionState == READING)
				FD_SET(fd, &_readFdSet);
			if ((*i)->connectionState == WRITING)
				FD_SET(fd, &_writeFdSet);
		}
	}
}

void RewriteServer::_handleSelect() {
	// check event bus for events
	if (FD_ISSET(_eventBus.getReadFD(), &_readFdSet)) {
		ServerEventBus::Events	event = _eventBus.getPostedEvent();
		if (event == ServerEventBus::CLIENT_STATE_UPDATED)
			return; // will loop around and recreate the sets
	}

	// check if new clients
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i) {
		FD	fd = (*i)->getFD();
		if (FD_ISSET(fd, &_readFdSet)) {
			// accept new client
			HTTPClient	*newClient = (*i)->acceptClient();
			_clients.push_back(newClient);
		}
	}

	// check client read/write
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		FD	fd = (*i)->getFd();
		if (FD_ISSET(fd, &_readFdSet)) {
			_handlers.handleClient(**i, HandlerHolder::READ);
		}
		else if (FD_ISSET(fd, &_writeFdSet)) {
			_handlers.handleClient(**i, HandlerHolder::WRITE);
		}
	}

	// check stdin for terminal data
	if (FD_ISSET(_termClient.getFd(), &_readFdSet)) {
		TerminalClient::TerminalCommandState	parseState = _termClient.readNewData();
		while (parseState == TerminalClient::FOUND_LINE) {
			_termResponder.respond(_termClient.takeLine());
			parseState = _termClient.parseState();
		}
	}
}

void RewriteServer::_clientCleanup() {
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		if ((*i)->connectionState != CLOSED)
			continue;
		HTTPClient	*client = *i;
		_clients.erase(i);
		close(client->getFd());
		delete client;
	}
}

void RewriteServer::startServer(config::RootBlock *c) {
	configuration = c;

	// start listeners
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i) {
		(*i)->start();
	}

	while (true) {
		// cleanup old clients (if any)
		_clientCleanup();

		// create fd sets for select
		_createFdSets();

		// run select
		SelectReturn	ret = _runSelect();
		if (ret == ERROR)
			throw IoSelectingFailed();
		else if (ret == TIMEOUT) {
			// TODO handle timeouts
			continue;
		}

		// handle select output
		_handleSelect();
	}
}

void RewriteServer::addListener(TCPListener *listener) {
	_listeners.push_back(listener);
}

void RewriteServer::addHandler(AHandler *handler) {
	handler->setParser(&_httpParser);
	handler->setResponder(&_httpResponder);
	handler->setEventBus(&_eventBus);
	_handlers.addHandler(handler);
}

RewriteServer::~RewriteServer() {
	// listeners
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i)
		delete *i;
	// left over clients
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i)
		delete *i;
	// other
	delete configuration;
}

RewriteServer::RewriteServer(): _readFdSet(), _writeFdSet(), _maxFd(), _termClient(STDIN_FILENO) {

}
