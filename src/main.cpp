#include <iostream>
#include "log/Loggable.hpp"
#include "server/Server.hpp"
#include "server/TCPListener.hpp"
#include "server/StandardHandler.hpp"
#include "server/HTTPParser.hpp"

using namespace NotApache;

int main() {
	log::Logger logger = std::cout;
	logger.setFlags(log::Flags::Debug | log::Flags::Color);

	Server	server;
	server.setLogger(logger);

	TCPListener	*listener = new TCPListener(8080);
	server.addListener(listener);

	StandardHandler	*handler = new StandardHandler();
	server.addHandler(handler);

	HTTPParser	*parser = new HTTPParser();
	server.addParser(parser);

	server.serve();
	return 0;
}
