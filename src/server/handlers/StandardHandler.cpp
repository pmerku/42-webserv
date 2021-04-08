//
// Created by jelle on 3/3/2021.
//

#include "server/handlers/StandardHandler.hpp"
#include "server/global/GlobalLogger.hpp"
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/wait.h>

using namespace NotApache;

const int	StandardHandler::_bufferSize = 1024;

// TODO broken pipe??
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
		FD fileFd = client.getAssociatedFd(0).fd;
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
				globalLogger.logItem(logger::ERROR, "Failed to read from associated file FD");
				globalLogger.logItem(logger::ERROR, std::string("Failed to read: ") + std::strerror(errno)); // TODO remove
				stopHandle(client);
				return;
			default:
				client.data.response.appendAssociatedData(buf, ret);
				stopHandle(client);
				return;
		}
	} else if (client.responseState == PROXY) {
		char	buf[_bufferSize+1];
		FD fileFd = client.getAssociatedFd(0).fd;
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
				globalLogger.logItem(logger::ERROR, "Failed to read from associated file FD");
				globalLogger.logItem(logger::ERROR, std::string("Failed to read: ") + std::strerror(errno)); // TODO remove
				stopHandle(client);
				return;
			default:
				client.proxy->response.appendResponseData(buf, ret);
				client.isHandled.lock();
				if (_parser->parse(client.proxy->response.data, &client) == HTTPParser::READY_FOR_WRITE) {
					client.connectionState = WRITING;
					client.writeState = GOT_ASSOCIATED;
				}
				std::cout << client.proxy->response.data << std::endl;
				stopHandle(client, false);
				return;
		}
	}
	else if (client.responseState == CGI) {
		char	buf[_bufferSize+1];
		FD fileFd = client.getAssociatedFd(0).fd;
		ssize_t	ret = ::read(fileFd, buf, _bufferSize);
		switch (ret) {
			case 0:
			    // check for error exit codes
                if (::waitpid(client.cgi->pid, &client.cgi->status, WUNTRACED) > 0) {
                    if (WIFEXITED(client.cgi->status))
                        client.cgi->status = WEXITSTATUS(client.cgi->status);
                    else if (WIFSIGNALED(client.cgi->status)) {
                        client.cgi->status = WTERMSIG(client.cgi->status);
                    }
				    client.cgi->hasExited = true;
                }
				// has read everything
				client.isHandled.lock();
				client.connectionState = WRITING;
				client.writeState = GOT_ASSOCIATED;
				stopHandle(client, false);
				return;
			case -1:
				globalLogger.logItem(logger::ERROR, "Failed to read from associated file FD");
				globalLogger.logItem(logger::ERROR, std::string("Failed to read: ") + std::strerror(errno)); // TODO remove
				stopHandle(client);
				return;
			default:
				client.cgi->response.appendResponseData(buf, ret);
				client.isHandled.lock();
				if (_parser->parse(client.cgi->response.data, &client) == HTTPParser::READY_FOR_WRITE) {
					client.connectionState = WRITING;
					client.writeState = GOT_ASSOCIATED;
				}
				std::cout << client.cgi->response.data << std::endl;
				stopHandle(client, false);
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
			globalLogger.logItem(logger::ERROR, "Failed to read from client");
			globalLogger.logItem(logger::ERROR, std::string("Failed to read: ") + std::strerror(errno)); // TODO remove
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

void StandardHandler::handleAssociatedWrite(HTTPClient &client) {
	globalLogger.logItem(logger::DEBUG, "Handling associated file descriptors");
	if (client.responseState == PROXY) {
		if (!client.proxy->request.hasProgress) {
			client.proxy->request.currentPacket = client.proxy->request.getRequest().begin();
			client.proxy->request.packetProgress = 0;
			client.proxy->request.hasProgress = true;
		}
		std::string::size_type	pos = client.proxy->request.packetProgress;
		std::string::size_type	len = client.proxy->request.currentPacket->size - pos;
		FD fileFd = client.getAssociatedFd(0).fd;
		ssize_t ret = ::write(fileFd, client.proxy->request.currentPacket->data + pos, len);
		switch (ret) {
			case -1:
				globalLogger.logItem(logger::ERROR, "Failed to write to server");
				globalLogger.logItem(logger::ERROR, std::string("Failed to write: ") + std::strerror(errno)); // TODO remove
				client.isHandled = false;
				return;
			case 0:
				// zero bytes is unlikely to happen, dont do anything if it does happen
				break;
			default:
				client.proxy->request.packetProgress += ret;
				if (client.proxy->request.packetProgress == client.proxy->request.currentPacket->size) {
					++client.proxy->request.currentPacket;
					client.proxy->request.packetProgress = 0;
				}
				if (client.proxy->request.currentPacket == client.proxy->request.getRequest().end()) {
					// wrote entire request, closing
					client.isHandled.lock();
					client.connectionState = ASSOCIATED_FD;
					client.setAssociatedFdMode(fileFd, associatedFD::READ);
					stopHandle(client, false);
					return;
				}
				break;
		}
	}
	else if (client.responseState == CGI) {
		utils::DataList *body = NULL;
		if (client.data.request.data.isChunked)
			body = &client.data.request.data.chunkedData;
		else
			body = &client.data.request.data.data;
		if (!client.data.request.hasProgress) {
			client.data.request.currentPacket = body->begin();
			client.data.request.packetProgress = 0;
			client.data.request.hasProgress = true;
		}
		std::string::size_type	pos = client.data.request.packetProgress;
		std::string::size_type	len = client.data.request.currentPacket->size - pos;
		FD fileFd = client.getAssociatedFd(1).fd;
		ssize_t ret = ::write(fileFd, client.data.request.currentPacket->data + pos, len);
		switch (ret) {
			case -1:
				std::cout << fileFd << std::endl;
				globalLogger.logItem(logger::ERROR, "Failed to write to server");
				globalLogger.logItem(logger::ERROR, std::string("Failed to write: ") + std::strerror(errno)); // TODO remove
				client.isHandled = false;
				return;
			case 0:
				// zero bytes is unlikely to happen, dont do anything if it does happen
				break;
			default:
				client.data.request.packetProgress += ret;
				if (client.data.request.packetProgress == client.data.request.currentPacket->size) {
					++client.data.request.currentPacket;
					client.data.request.packetProgress = 0;
				}
				if (client.data.request.currentPacket == body->end()) {
					// wrote entire request, closing
					client.isHandled.lock();
					client.removeAssociatedFd(fileFd);
					stopHandle(client, false);
					return;
				}
				break;
		}
	}
}

void StandardHandler::write(HTTPClient &client) {
	if (client.connectionState == ASSOCIATED_FD) {
		handleAssociatedWrite(client);
		stopHandle(client);
		return;
	}
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
				globalLogger.logItem(logger::ERROR, "Failed to write to client");
				globalLogger.logItem(logger::ERROR, std::string("Failed to write: ") + std::strerror(errno)); // TODO remove
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
