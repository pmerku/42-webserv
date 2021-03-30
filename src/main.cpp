#include <iostream>
#include "log/Logger.hpp"
#include "server/Server.hpp"
#include "config/ConfigParser.hpp"
#include <csignal>

#include "server/handlers/ThreadHandler.hpp"

using namespace NotApache;

static void handleSignals() {
	std::signal(SIGPIPE, SIG_IGN);
}

int main() {

	// setup loggers
	logger::Logger logger = std::cout;
	logger.setFlags(logger::Flags::Color | logger::Flags::Debug);

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
	{
		// only add unique host+port pairs
		std::vector<std::pair<std::string, int> >	uniquePairs;
		for (std::vector<config::ServerBlock *>::const_iterator i = config->getServerBlocks().begin(); i != config->getServerBlocks().end(); ++i) {
			bool alreadyAdded = false;
			for (std::vector<std::pair<std::string, int> >::const_iterator j = uniquePairs.begin(); j != uniquePairs.end(); ++j) {
				// TODO check extra zeros in host ip (127.0.0.1 is the same as 127.000.000.001)
				if (j->first == (*i)->getHost() && j->second == (*i)->getPort()) {
					alreadyAdded = true;
					break;
				}
			}
			if (!alreadyAdded)
				uniquePairs.push_back(std::pair<std::string, int>((*i)->getHost(), (*i)->getPort()));
		}

		for (std::vector<std::pair<std::string, int> >::const_iterator j = uniquePairs.begin(); j != uniquePairs.end(); ++j) {
			server.addListener(new TCPListener(j->second));
		}
	}

	// add workers
	if (config->getWorkerCount() == -1) // only use main thread
		server.addHandler(new StandardHandler());
	else {
		for (int i = 0; i < config->getWorkerCount(); ++i) {
			server.addHandler(new ThreadHandler());
		}
	}

	// start server
	handleSignals();
	try {
		server.startServer(config);
	} catch (std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, e.what()));
		return 1;
	}
	return 0;
}
