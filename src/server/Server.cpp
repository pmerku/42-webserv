//
// Created by jelle on 3/12/2021.
//

#include "server/Server.hpp"
#include "utils/intToString.hpp"
#include "server/global/GlobalConfig.hpp"
#include <unistd.h>
#include <errno.h>

using namespace NotApache;

Server::SelectReturn Server::_runSelect() {
	timeval	timeout = {};
	timeout.tv_sec = 5; // check timeouts every 5 seconds

	int ret = ::select(_maxFd + 1, &_readFdSet, &_writeFdSet, 0, &timeout);
	if (ret == 0)
		return TIMEOUT;
	else if (ret == -1)
		return errno == EINTR ? TIMEOUT : ERROR; // treat EINTR as TIMEOUT
	return SUCCESS;
}

void Server::_createFdSets() {
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
	if (!_termClient.isClosed()) {
		if (_termClient.getFd() > _maxFd) _maxFd = _termClient.getFd();
		FD_SET(_termClient.getFd(), &_readFdSet);
	}

	// add clients
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		(*i)->isHandled.lock();
		bool isBeingHandled = (*i)->isHandled.get();
		(*i)->isHandled.unlock();
		if (isBeingHandled) continue;
		if ((*i)->connectionState == READING || (*i)->connectionState == WRITING) {
			FD	fd = (*i)->getFd();
			if (fd > _maxFd) _maxFd = fd;
			if ((*i)->connectionState == READING)
				FD_SET(fd, &_readFdSet);
			else if ((*i)->connectionState == WRITING)
				FD_SET(fd, &_writeFdSet);
		}
		else if ((*i)->connectionState == ASSOCIATED_FD) {
			for (size_t j = 0; j < (*i)->getAssociatedFdLength(); ++j) {
				if ((*i)->getAssociatedFd(j).fd > _maxFd) _maxFd = (*i)->getAssociatedFd(j).fd;
				if ((*i)->getAssociatedFd(j).mode == associatedFD::READ)
					FD_SET((*i)->getAssociatedFd(j).fd, &_readFdSet);
				else
					FD_SET((*i)->getAssociatedFd(j).fd, &_writeFdSet);
			}
		}
	}
}

void Server::_handleSelect() {
	// check event bus for events
	if (FD_ISSET(_eventBus.getReadFD(), &_readFdSet)) {
		ServerEventBus::Events	event = _eventBus.getPostedEvent();
		globalLogger.logItem(logger::DEBUG, "Event bus triggered");
		if (event == ServerEventBus::CLIENT_STATE_UPDATED)
			return; // will loop around and recreate the sets
	}

	// check if new clients
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i) {
		FD	fd = (*i)->getFD();
		if (FD_ISSET(fd, &_readFdSet)) {
			// accept new client
			HTTPClient	*newClient = (*i)->acceptClient();
			globalLogger.logItem(logger::DEBUG, "Client connected");
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
		else {
			for (std::vector<FD>::size_type j = 0; j < (*i)->getAssociatedFdLength(); j++) {
				if (FD_ISSET((*i)->getAssociatedFd(j).fd, &_readFdSet)) {
					_handlers.handleClient(**i, HandlerHolder::READ);
				}
				else if (FD_ISSET((*i)->getAssociatedFd(j).fd, &_writeFdSet)) {
					_handlers.handleClient(**i, HandlerHolder::WRITE);
				}
			}
		}
	}

	// check stdin for terminal data
	if (FD_ISSET(_termClient.getFd(), &_readFdSet)) {
		TerminalClient::TerminalCommandState	parseState = _termClient.readNewData();
		while (parseState == TerminalClient::FOUND_LINE) {
			_termResponder.respond(_termClient.takeLine(), this);
			parseState = _termClient.parseState();
		}
	}
}

void Server::_clientCleanup() {
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		(*i)->isHandled.lock();
		// if not handled, do a timeout check
		if (!(*i)->isHandled.get()) {
			(*i)->timeout(false);
		}

		// if closed & is not being handled. then set isHandled to true and close client
		bool isClosed = (*i)->connectionState == CLOSED;
		if (_shouldShutdown) isClosed = true; // always treat client as closed if it should shutdown
		if (isClosed) {
			if ((*i)->isHandled.get())
				isClosed = false;
			else
				(*i)->isHandled.setNoLock(true);
		}
		(*i)->isHandled.unlock();

		if (!isClosed)
			continue;
		std::string start = "Closed client connection #";
		if ((*i)->getTimeDiff() >= (*i)->getTimeoutAfter())
			start = "Closed (TIME OUT) client connection #";
		globalLogger.logItem(logger::INFO, start + utils::intToString((int)(*i)->clientCount));
		close((*i)->getFd());
		delete *i;
		*i = 0;
	}
	_clients.remove(0);
}

void Server::startServer(config::RootBlock *c) {
	configuration = c;

	// start listeners
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i) {
		globalLogger.logItem(logger::INFO, "Starting listener");
		(*i)->start();
	}

	globalLogger.logItem(logger::INFO, "Initialisation finished, now open for connections");
	while (true) {
		// cleanup old clients (if any)
		_clientCleanup();

		// shutdown if no more clients are active
		if (_shouldShutdown && _clients.empty())
			return;

		// create fd sets for select
		_createFdSets();

		// run select
		SelectReturn	ret = _runSelect();
		if (ret == ERROR)
			throw IoSelectingFailed();
		else if (ret == TIMEOUT)
			continue; // go to next iteration (cleanup will act on timeouts)

		// handle select output
		_handleSelect();
	}
}

void Server::addListener(TCPListener *listener) {
	_listeners.push_back(listener);
}

void Server::addHandler(AHandler *handler) {
	handler->setParser(&_httpParser);
	handler->setResponder(&_httpResponder);
	handler->setEventBus(&_eventBus);
	_handlers.addHandler(handler);
}

void Server::setLogger(logger::Logger &logger) {
	globalLogger.setLogger(logger);
}

Server::~Server() {
	// listeners
	for (std::vector<TCPListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i)
		delete *i;
	// left over clients
	for (std::list<HTTPClient*>::iterator i = _clients.begin(); i != _clients.end(); ++i)
		delete *i;
	// other
	delete configuration;
}

Server::Server(): _readFdSet(), _writeFdSet(), _maxFd(), _shouldShutdown(false), _termClient(STDIN_FILENO) {}

void Server::shutdownServer() {
	globalLogger.logItem(logger::INFO, "Received shutdown signal, gracefully shutting down");
	_shouldShutdown = true;
	// trigger a select iteration so it doesnt wait for a timeout
	_eventBus.postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
}
