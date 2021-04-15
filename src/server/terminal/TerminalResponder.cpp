//
// Created by jelle on 3/13/2021.
//

#include "server/terminal/TerminalResponder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalPlugins.hpp"
#include "libs/duktape/duktape.h"

using namespace NotApache;

void TerminalResponder::respond(const std::string &str) {
	(void)str;


	return;

	// TODO handle terminal command
//	if (str == "load page_404_gen") {
//		plugin::PluginContainer::pluginIterator it = globalPlugins.find("page_404_gen");
//		if (it != globalPlugins.end())
//			it->second = true;
//	}
//	else if (str == "unload page_404_gen") {
//		plugin::PluginContainer::pluginIterator it = globalPlugins.find("page_404_gen");
//		if (it != globalPlugins.end())
//			it->second = false;
//	}
	globalLogger.logItem(logger::INFO, "Handled terminal command :)");
}
