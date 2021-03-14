//
// Created by jelle on 3/3/2021.
//

#include "rewrite-server/handlers/StandardHandler.hpp"
#include <unistd.h>

using namespace NotApacheRewrite;

void StandardHandler::read(HTTPClient &client) {
	char	buf[1025];

	ssize_t	ret = ::read(client.getFd(), buf, sizeof(buf)-1);
	switch (ret) {
		case 0:
			// connection closed
			client.connectionState = CLOSED;
			_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
			return;
		case -1:
			// TODO error handling
			return;
		default:
			// packet found, reading
			buf[ret] = 0; // make cstr out of it by setting 0 as last char
			// TODO handle new data
			break;
	}

	// parsing
	// TODO parsing
	client.connectionState = WRITING;
	_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
}

void StandardHandler::write(HTTPClient &client) {
	if (client.writeState == NO_RESPONSE) {
		// TODO generate response
		client.writeState = IS_WRITING;
	}

	if (client.writeState == IS_WRITING) {
		// TODO write response
		ssize_t ret = ::write(client.getFd(), "Hello world", 11);
		switch (ret) {
			case -1:
				// TODO fail write
				break;
			case 0:
				// TODO 0 bytes written
				break;
			default:
				// TODO handle write return
				if (ret < 11) {
					// wrote entire response, close
					client.connectionState = CLOSED;
					_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
					return;
				}
				break;
		}
	}
}

StandardHandler::StandardHandler(): AHandler() {}
