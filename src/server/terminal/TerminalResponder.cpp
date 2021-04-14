//
// Created by jelle on 3/13/2021.
//

#include "server/terminal/TerminalResponder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalPlugins.hpp"

using namespace NotApache;

void TerminalResponder::respond(const std::string &str) {
	// TODO handle terminal command
	if (str == "load page_404_gen") {
		plugin::PluginContainer::pluginIterator it = globalPlugins.find("page_404_gen");
		if (it != globalPlugins.end())
			it->second = true;
	}
	else if (str == "unload page_404_gen") {
		plugin::PluginContainer::pluginIterator it = globalPlugins.find("page_404_gen");
		if (it != globalPlugins.end())
			it->second = false;
	}

	if (str == "load json_stat_api") {
		plugin::PluginContainer::pluginIterator it = globalPlugins.find("json_stat_api");
		if (it != globalPlugins.end()) {
			globalLogger.logItem(logger::INFO, "Plugin set to true");
			it->second = true;
		}
	}
	else if (str == "unload json_stat_api") {
		plugin::PluginContainer::pluginIterator it = globalPlugins.find("json_stat_api");
		if (it != globalPlugins.end())
			it->second = false;
	}
	
	if (str == "help") {
		std::cout	<< "command options:" 			<< std::endl
					<< "\t- exit" 					<< std::endl
					<< "\t- help" 					<< std::endl

					<< "load/unload plugins:" 		<< std::endl
					<< "\t- load page_404_gen"		<< std::endl
					<< "\t- unload page_404_gen"	<< std::endl
					<< "\t- load json_stat_api" 		<< std::endl
					<< "\t- unload json_stat_api" 		<< std::endl;
	}

	globalLogger.logItem(logger::INFO, "Handled terminal command :)");
}
