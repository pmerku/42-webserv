//
// Created by pmerku on 14/04/2021.
//

#ifndef GLOBALPLUGINS_HPP
#define GLOBALPLUGINS_HPP

#include "plugins/Plugin.hpp"
#include <map>

namespace plugin {

	class PluginContainer {
	public:
		typedef std::map<Plugin*, bool> pluginMap;
		typedef pluginMap::iterator 	pluginIterator;

	private:
		pluginMap	_plugin;

	public:
		explicit PluginContainer(const pluginMap &map);
		~PluginContainer();

		pluginIterator find(const std::string &name);
		void add(Plugin *plugin);

		pluginIterator begin();
		pluginIterator end();
	};
}

namespace NotApache {
	extern plugin::PluginContainer globalPlugins;
}

#endif //GLOBALPLUGINS_HPP
