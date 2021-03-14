#include <iostream>
#include "log/Logger.hpp"
#include "server/Server.hpp"
#include "config/ConfigParser.hpp"

#include "server/handlers/ThreadHandler.hpp"

using namespace NotApache;

int main() {
	// setup loggers
	logger::Logger logger = std::cout;
	logger.setFlags(logger::Flags::Debug | logger::Flags::Color);

	// parse configuration
	config::ConfigParser parser;
	parser.setLogger(logger);
	config::RootBlock *config;
	try {
		config = parser.parseFile("../resources/example-configs/basic.conf");
	} catch (const std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, std::string("Config could not be parsed: ") + e.what()));
		return 1;
	}

	// create server
	Server server;
	server.setLogger(logger);

	// add server listeners
	for (std::vector<config::ServerBlock*>::const_iterator i = config->getServerBlocks().begin(); i != config->getServerBlocks().end(); ++i) {
		server.addListener(new TCPListener((*i)->getPort()));
	}

	// workers
	if (config->getWorkerCount() == -1)
		server.addHandler(new StandardHandler());
	else {
		for (int i = 0; i < config->getWorkerCount(); ++i) {
			server.addHandler(new ThreadHandler());
		}
	}

	// start server
	try {
		server.startServer(config);
	} catch (std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, e.what()));
		return 1;
	}
	return 0;
}
