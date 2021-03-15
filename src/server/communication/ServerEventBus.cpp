//
// Created by jelle on 3/13/2021.
//

#include "server/communication/ServerEventBus.hpp"

using namespace NotApache;

ServerEventBus::ServerEventBus() {
	int	pipeFds[2];
	if (::pipe(pipeFds) == -1)
		throw PipeCreationFailedException();
	_readEnd = pipeFds[0];
	_writeEnd = pipeFds[1];
}

ServerEventBus::ServerEventBus(const ServerEventBus &cpy): _writeEnd(cpy._writeEnd), _readEnd(cpy._readEnd) {}

FD	ServerEventBus::getReadFD() {
	return _readEnd;
}

ServerEventBus::Events ServerEventBus::getPostedEvent() {
	char c;
	if (::read(_readEnd, &c, 1) == -1)
		throw FailedToGetEvent();
	return static_cast<Events>(c);
}

void	ServerEventBus::postEvent(ServerEventBus::Events event) {
	char c = static_cast<char>(event);
	if (::write(_writeEnd, &c, 1) == -1)
		throw FailedToPostEvent();
}

ServerEventBus::~ServerEventBus() {
	::close(_readEnd);
	::close(_writeEnd);
}
