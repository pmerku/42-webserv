#include <iostream>
#include "log/logger.hpp"
#include "server/Server.hpp"

// listeners
#include "server/listeners/TCPListener.hpp"
#include "server/listeners/TerminalListener.hpp"

// handlers
#include "server/handlers/StandardHandler.hpp"

// responders
#include "server/responders/HTTPResponder.hpp"
#include "server/responders/TerminalResponder.hpp"

// parsers
#include "server/parsers/HTTPParser.hpp"
#include "server/parsers/TerminalParser.hpp"

using namespace NotApache;

int main() {
	log::Logger logger = std::cout;
	logger.setFlags(log::Flags::Debug | log::Flags::Color);

	Server server;
	server.setLogger(logger);

	TCPListener *portListen = new TCPListener(8080);
	TerminalListener *termListen = new TerminalListener();
	server.addListener(portListen);
	server.addListener(termListen);

	StandardHandler *handler = new StandardHandler();
	server.addHandler(handler);

	HTTPResponder *httpResponder = new HTTPResponder();
	TerminalResponder *terminalResponder = new TerminalResponder();
	server.addResponder(httpResponder);
	server.addResponder(terminalResponder);

	HTTPParser *httpParser = new HTTPParser();
	TerminalParser *terminalParser = new TerminalParser();
	server.addParser(httpParser);
	server.addParser(terminalParser);

	server.serve();
	return 0;
}
