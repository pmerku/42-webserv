//
// Created by jelle on 3/13/2021.
//

#include "server/terminal/TerminalResponder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalPlugins.hpp"
#include "utils/split.hpp"
#include "server/Server.hpp"

using namespace NotApache;

const std::string TerminalResponder::_help("\
	command options:\n\
		- exit\n\
		- help\n\
	load/unload plugins:\n\
		- load page_404_gen\n\
		- unload page_404_gen\n\
		- load json_stat_api\n\
		- unload json_stat_api"
	);

void TerminalResponder::respond(const std::string &str, Server* server) {
	std::vector<std::string> command = utils::split(str, " ");
	
	if (command[0] == "load") {
		plugin::PluginContainer::pluginIterator it = globalPlugins.find(command[1]);
		if (it != globalPlugins.end()) {
			if (it->second == true)
				globalLogger.logItem(logger::INFO, "Plugin already loaded");
			else {
				it->second = true;
				globalLogger.logItem(logger::INFO, "Plugin loaded");
			}
		}
	}
	else if (command[0] == "unload") {
		plugin::PluginContainer::pluginIterator it = globalPlugins.find(command[1]);
		if (it != globalPlugins.end()) {
			if (it->second == false)
				globalLogger.logItem(logger::INFO, "Plugin already unloaded");
			else {
				it->second = false;
				globalLogger.logItem(logger::INFO, "Plugin unloaded");
			}
		}
		else {
			globalLogger.logItem(logger::ERROR, "Plugin not found");
			return ;
		}
	}
	else if (command[0] == "exit")
		server->shutdownServer();
	else {
		if (command[0] != "help")
			globalLogger.logItem(logger::INFO, "Invalid command...");
		globalLogger.logItem(logger::INFO, _help);
	}

	globalLogger.logItem(logger::INFO, "Handled terminal command :)");
}
