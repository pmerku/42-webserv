//
// Created by jelle on 3/3/2021.
//

#include "server/handlers/StandardHandler.hpp"
#include "server/global/GlobalLogger.hpp"
#include <unistd.h>
#include <cerrno>
#include <cstring>

using namespace NotApache;

const int	StandardHandler::_bufferSize = 1024;

void StandardHandler::stopHandle(HTTPClient &client, bool shouldLock) {
	if (shouldLock) client.isHandled.lock();
	client.timeout(false);
	_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
	client.isHandled.setNoLock(false);
	client.isHandled.unlock();
}

void StandardHandler::handleAssociatedRead(HTTPClient &client) {
	globalLogger.logItem(logger::DEBUG, "Handling associated file descriptors");
	if (client.responseState == FILE) {
		char	buf[_bufferSize+1];
		FD fileFd = client.getAssociatedFd(0);
		ssize_t	ret = ::read(fileFd, buf, _bufferSize);
		switch (ret) {
			case 0:
				// has read everything
				client.isHandled.lock();
				client.connectionState = WRITING;
				client.writeState = GOT_ASSOCIATED;
				stopHandle(client, false);
				return;
			case -1:
				globalLogger.logItem(logger::DEBUG, "Failed to read from associated file FD");
				std::cout << "Failed to read: " << std::strerror(errno) << std::endl;
				stopHandle(client);
				return;
			default:
				client.data.response.appendAssociatedData(buf, ret);
				stopHandle(client);
				return;
		}
	}
}

void StandardHandler::read(HTTPClient &client) {
	char	buf[_bufferSize+1];

	if (client.connectionState == ASSOCIATED_FD) {
		handleAssociatedRead(client);
		return;
	}

	ssize_t	ret = ::read(client.getFd(), buf, _bufferSize);
	switch (ret) {
		case 0:
			// connection closed
			client.isHandled.lock();
			client.connectionState = CLOSED;
			stopHandle(client, false);
			return;
		case -1:
			globalLogger.logItem(logger::DEBUG, "Failed to read from client");
			std::cout << "Failed to read: " << std::strerror(errno) << std::endl;
			stopHandle(client);
			return;
		default:
			// packet found, reading
			client.data.request.appendRequestData(buf, ret);
			break;
	}

	// parsing
	client.isHandled.lock();
	HTTPParser::ParseState parseRet = _parser->parse(client);
	std::cout << client.data.request.data << std::endl;
	if (parseRet == HTTPParser::READY_FOR_WRITE) {
		client.connectionState = WRITING;
	}
	stopHandle(client, false);
}

void StandardHandler::write(HTTPClient &client) {
	if (client.writeState == NO_RESPONSE) {
		_responder->generateResponse(client);
		if (client.connectionState == ASSOCIATED_FD) {
			stopHandle(client);
			return;
		}
		client.writeState = IS_WRITING;
	}
	else if (client.writeState == GOT_ASSOCIATED) {
		_responder->generateAssociatedResponse(client);
		client.writeState = IS_WRITING;
	}

	if (client.writeState == IS_WRITING) {
		if (!client.data.response.hasProgress) {
			client.data.response.currentPacket = client.data.response.getResponse().begin();
			client.data.response.packetProgress = 0;
			client.data.response.hasProgress = true;
		}
		std::string::size_type	pos = client.data.response.packetProgress;
		std::string::size_type	len = client.data.response.currentPacket->size - pos;
		ssize_t ret = ::write(client.getFd(), client.data.response.currentPacket->data + pos, len);
		switch (ret) {
			case -1:
				globalLogger.logItem(logger::DEBUG, "Failed to write to client");
				client.isHandled = false;
				return;
			case 0:
				// zero bytes is unlikely to happen, dont do anything if it does happen
				break;
			default:
				client.data.response.packetProgress += ret;
				if (client.data.response.packetProgress == client.data.response.currentPacket->size) {
					++client.data.response.currentPacket;
					client.data.response.packetProgress = 0;
				}
				if (client.data.response.currentPacket == client.data.response.getResponse().end()) {
					// wrote entire response, closing
					client.isHandled.lock();
					client.connectionState = CLOSED;
					stopHandle(client, false);
					return;
				}
				break;
		}
	}
	stopHandle(client);
}

StandardHandler::StandardHandler(): AHandler() {}
