#include <iostream>
#include "log/Logger.hpp"
#include "server/Server.hpp"
#include "config/ConfigParser.hpp"
#include <csignal>
#include "utils/ArgParser.hpp"
#include "server/handlers/ThreadHandler.hpp"

using namespace NotApache;

static void handleSignals() {
	std::signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char *argv[]) {
	logger::Logger logger = std::cout;
	utils::ArgParser args;
	try {
		args = utils::ArgParser(argc, argv);
	} catch (const utils::ArgParser::ArgParserException &e) {
		std::cout << utils::ArgParser::printUsage() << std::endl;
		logger.log(logger::LogItem(logger::ERROR, e.what()));
		return 1;
	} catch (const std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, std::string("Unhandled exception: ") + e.what()));
		return 1;
	}

	// setup loggers
	logger::Flags::flagType flags = 0;
	if (args.colorPrint())
		flags |= logger::Flags::Color;
	if (args.verbosePrint())
		flags |= logger::Flags::Debug;
	logger.setFlags(flags);

	// parse configuration
	config::ConfigParser parser;
	parser.setLogger(logger);
	config::RootBlock *config;
	try {
		config = parser.parseFile(args.configFile());
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
		std::vector<std::pair<long, int> >	uniquePairs;
		for (std::vector<config::ServerBlock *>::const_iterator i = config->getServerBlocks().begin(); i != config->getServerBlocks().end(); ++i) {
			bool alreadyAdded = false;
			for (std::vector<std::pair<long, int> >::const_iterator j = uniquePairs.begin(); j != uniquePairs.end(); ++j) {
				if (j->first == (*i)->getHost() && j->second == (*i)->getPort()) {
					alreadyAdded = true;
					break;
				}
			}
			if (!alreadyAdded)
				uniquePairs.push_back(std::pair<long, int>((*i)->getHost(), (*i)->getPort()));
		}

		for (std::vector<std::pair<long, int> >::const_iterator j = uniquePairs.begin(); j != uniquePairs.end(); ++j) {
			server.addListener(new TCPListener(j->second, j->first));
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
