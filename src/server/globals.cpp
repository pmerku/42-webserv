//
// Created by jelle on 3/14/2021.
//

#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"
#include "server/global/GlobalPlugins.hpp"
#include "utils/CreateMap.hpp"
#include "plugins/PageGenerator.hpp"
#include "plugins/JsonStatAPI.hpp"

namespace NotApache {
	config::RootBlock	*configuration = 0;
	logger::ILoggable	globalLogger = logger::ILoggable();

	plugin::PluginContainer globalPlugins(utils::CreateMap<plugin::Plugin*, bool>
	        (new plugin::PageGenerator(), false)
	        (new plugin::JsonStatAPI(), true) // TODO set back to false
	        (new plugin::Plugin("fuck_brainfuck"), false)
	        (new plugin::Plugin("replace_text"), false)
	        (new plugin::Plugin("js_executer"), false)
	        );
}

using namespace plugin;

PluginContainer::PluginContainer(const pluginMap &map) : _plugin(map) { }

PluginContainer::~PluginContainer() {
	for (pluginIterator it = _plugin.begin(); it != _plugin.end(); ++it) {
		delete it->first;
	}
}

PluginContainer::pluginIterator PluginContainer::find(const std::string &name) {
	for (pluginIterator it = _plugin.begin(); it != _plugin.end(); ++it) {
		if (*(it->first) == name)
			return it;
	}
	return _plugin.end();
}

void PluginContainer::add(Plugin *plugin) {
	_plugin[plugin] = false;
}

PluginContainer::pluginIterator PluginContainer::begin() {
	return _plugin.begin();
}

PluginContainer::pluginIterator PluginContainer::end() {
	return _plugin.end();
}
