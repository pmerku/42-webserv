//
// Created by jelle on 3/3/2021.
//

#include "server/handlers/StandardHandler.hpp"
#include <unistd.h>

using namespace NotApache;

void StandardHandler::read(Client &client) {
	char	buf[1025];

	ssize_t	ret = ::read(client.getFD(), buf, sizeof(buf)-1);
	switch (ret) {
		case 0:
			// connection closed
			logItem(log::DEBUG, "Reached EOF of client");
			logItem(log::DEBUG, "Client data:\n" + client.getRequest());
			client.close();
			return;
		case -1:
			// error reading
			logItem(log::WARNING, "Failed to read from client");
			logItem(log::DEBUG, client.getRequest());
			return;
		default:
			// packet found, reading
			buf[ret] = 0; // make cstr out of it by settings 0 as last char
			client.appendRequest(buf);
			break;
	}

	// parsing
	AParser::formatState state = AParser::runFormatChecks(*_parsers, client);
	if (state == AParser::FINISHED) {
		// has read full data, start responding. client now contains data type
		logItem(log::DEBUG, "Client data has been parsed");
		client.setState(WRITING);
		client.setResponseState(IS_RESPONDING);
		return;
	}
	else if (state == AParser::PARSE_ERROR) {
		logItem(log::DEBUG, "Client data cannot be parsed");
		client.setState(WRITING);
		client.setResponseState(PARSE_ERROR);
		return;
	}
}

void StandardHandler::write(Client &client) {
	if (client.getResponseState() == IS_RESPONDING || client.getResponseState() == PARSE_ERROR)
		respond(client);
	if (client.getResponseState() == ERRORED) {
		client.setResponse("Whoops, something went wrong :(\n");
		client.setResponseState(IS_WRITING);
		client.setResponseIndex(0);
	}

	if (client.getResponseState() == IS_WRITING) {
		std::string response = client.getResponse();
		ssize_t	writeLength = response.length() - client.getResponseIndex();
		ssize_t ret = ::write(client.getFD(), response.c_str() + client.getResponseIndex(), writeLength);
		switch (ret) {
			case -1:
				logItem(log::ERROR, "Failed to write to client");
				return;
			case 0:
				logItem(log::DEBUG, "Encountered 0 for write");
				return;
			default:
				client.setResponseIndex(client.getResponseIndex() + ret);
				if (client.getResponseIndex() == response.length()) {
					// wrote entire response, close
					client.close();
					return;
				}
				break;
		}
	}
}

StandardHandler::StandardHandler(): AHandler() {}
