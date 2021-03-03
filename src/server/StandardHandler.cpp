//
// Created by jelle on 3/3/2021.
//

#include "server/StandardHandler.hpp"
#include <unistd.h>
#include <sys/socket.h>

using namespace NotApache;

void StandardHandler::read(Client &client) {
	char	buf[1025];

	ssize_t	ret = recv(client.getFD(), buf, sizeof(buf)-1, 0);
	switch (ret) {
		case 0:
			// connection closed
			logItem(log::DEBUG, "Reached EOF of client");
			logItem(log::DEBUG, "Client data:\n" + client.getRequest());
			// TODO somehow delete client
			close(client.getFD());
			client.setState(CLOSED);
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
	AParser::parseState state = AParser::runParsers(*_parsers, client);
	if (state == AParser::FINISHED) {
		// has read full data, start responding
		logItem(log::DEBUG, "Client data has been parsed, writing");
		client.setState(WRITING);
		return;
	}
	else if (state == AParser::PARSE_ERROR) {
		logItem(log::DEBUG, "Client data cannot be parsed, writing error");
		client.setState(WRITING);
		return;
	}
}

// TODO write
void StandardHandler::write(Client &client) {
	std::string response = "Hello world\n";
	ssize_t ret = send(client.getFD(), response.c_str(), response.length(), 0);
	switch (ret) {
		case -1:
			logItem(log::ERROR, "Failed to write to client");
			return;
		case 0:
			logItem(log::DEBUG, "Encountered 0 for write");
			return;
		default:
			if (ret < (long int)response.length()) {
				logItem(log::DEBUG, "Not writing fully");
			}
			break;
	}
	client.setState(CLOSED);
	close(client.getFD());
}

StandardHandler::StandardHandler(): AHandler() {}
