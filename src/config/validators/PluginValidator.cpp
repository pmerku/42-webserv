//
// Created by jelle on 3/30/2021.
//

#include "config/validators/PluginValidator.hpp"
#include "server/global/GlobalPlugins.hpp"
#include "utils/stoi.hpp"

using namespace config;

PluginValidator::PluginValidator(int argnum): AConfigValidator(false), _argnum(argnum) {}

void PluginValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum)
		ERROR_THROW(PluginValidatorException(line, &block));
	const std::string &plugin = line.getArg(_argnum);
	for (plugin::PluginContainer::pluginIterator it = NotApache::globalPlugins.begin(); it != NotApache::globalPlugins.end(); ++it) {
		if (*(it->first) == plugin)
			return; // found valid plugin
	}
	// not found, invalid
	ERROR_THROW(PluginValidatorException(line, &block));
}
