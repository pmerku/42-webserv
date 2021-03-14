#include <iostream>
#include "log/Logger.hpp"
#include "server/Server.hpp"
#include "config/ConfigParser.hpp"

// listeners
#include "server/listeners/TCPListener.hpp"
#include "server/listeners/TerminalListener.hpp"

// handlers
#include "server/handlers/StandardHandler.hpp"
#include "server/handlers/ThreadHandler.hpp"

// responders
#include "server/responders/HTTPResponder.hpp"
#include "server/responders/TerminalResponder.hpp"

// parsers
#include "server/parsers/HTTPParser.hpp"
#include "server/parsers/TerminalParser.hpp"

using namespace NotApache;

int main() {
	logger::Logger logger = std::cout;
	logger.setFlags(logger::Flags::Debug | logger::Flags::Color);

	config::ConfigParser parser;
	parser.setLogger(logger);
	config::RootBlock *config;
	try {
		config = parser.parseFile("../resources/example-configs/basic.conf");
	} catch (const std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, std::string("Config could not be parsed: ") + e.what()));
		return 1;
	}

	Server server;
	server.setLogger(logger);

	// add server listeners
	for (std::vector<config::ServerBlock*>::const_iterator i = config->getServerBlocks().begin(); i != config->getServerBlocks().end(); ++i) {
		server.addListener(new TCPListener((*i)->getPort()));
	}
	server.addListener(new TerminalListener());

	// workers
	if (config->getWorkerCount() == -1)
		server.addHandler(new StandardHandler());
	else {
		for (int i = 0; i < config->getWorkerCount(); ++i) {
			server.addHandler(new ThreadHandler());
		}
	}

	server.addResponder(new HTTPResponder());
	server.addResponder(new TerminalResponder());

	server.addParser(new HTTPParser());
	server.addParser(new TerminalParser());

	try {
		server.serve();
	} catch (std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, e.what()));
	}
	return 0;
}
