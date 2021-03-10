//
// Created by jelle on 3/3/2021.
//

#include "server/handlers/StandardHandler.hpp"
#include <unistd.h>

using namespace NotApache;

void StandardHandler::read(Client &client) {
	char	buf[1025];

	ssize_t	ret = client.read(buf, sizeof(buf)-1);
	switch (ret) {
		case 0:
			// connection closed
			logItem(logger::DEBUG, "Reached EOF of client");
			logItem(logger::DEBUG, "Client data:\n" + client.getRequest());
			client.close(true);
			finish(client);
			return;
		case -1:
			// TODO error WOULD_BLOCK on terminal EOF with still data to read (example "command here\n unfinished(EOF)")
			// error reading
			logItem(logger::WARNING, "Failed to read from client");
			logItem(logger::DEBUG, client.getRequest());
			finish(client);
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
		logItem(logger::DEBUG, "Client data has been parsed");
		client.setState(WRITING);
		client.setResponseState(IS_RESPONDING);
		finish(client);
		return;
	}
	else if (state == AParser::PARSE_ERROR) {
		logItem(logger::DEBUG, "Client data cannot be parsed");
		client.setState(WRITING);
		client.setResponseState(PARSE_ERROR);
		finish(client);
		return;
	}

	// handle timeout
	client.timeout();
	finish(client);
}

void StandardHandler::write(Client &client) {
	switch (client.getResponseState()) {
		case IS_RESPONDING:
		case PARSE_ERROR:
		case TIMED_OUT:
			respond(client);
		default:
			break;
	}
	if (client.getResponseState() == ERRORED) {
		client.setResponse("Whoops, something went wrong :(\n");
		client.setResponseState(IS_WRITING);
		client.setResponseIndex(0);
	}

	if (client.getResponseState() == IS_WRITING) {
		std::string response = client.getResponse();
		ssize_t	writeLength = response.length() - client.getResponseIndex();
		ssize_t ret = client.write(response.c_str() + client.getResponseIndex(), writeLength);
		switch (ret) {
			case -1:
				logItem(logger::ERROR, "Failed to write to client");
				break;
			case 0:
				logItem(logger::DEBUG, "Encountered 0 for write");
				break;
			default:
				client.setResponseIndex(client.getResponseIndex() + ret);
				if (client.getResponseIndex() == response.length()) {
					// wrote entire response, close
					client.close(false);
					break;
				}
				break;
		}
	}
	finish(client);
}

StandardHandler::StandardHandler(): AHandler() {}

void StandardHandler::finish(Client &client) {
	client._isHandled = false;
}
