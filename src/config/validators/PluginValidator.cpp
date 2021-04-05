//
// Created by jelle on 3/30/2021.
//

#include "config/validators/PluginValidator.hpp"
#include "utils/stoi.hpp"
#include "utils/CreateVector.hpp"

using namespace config;

const std::vector<std::string>	PluginValidator::_plugins = utils::CreateVector<std::string>("");

PluginValidator::PluginValidator(int argnum): AConfigValidator(false), _argnum(argnum) {}

void PluginValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum)
		ERROR_THROW(PluginValidatorException(line, &block));
	const std::string &plugin = line.getArg(_argnum);
	for (std::vector<std::string>::const_iterator it = _plugins.begin(); it != _plugins.end(); ++it) {
		if (*it == plugin)
			return; // found valid plugin
	}
	// not found, invalid
	ERROR_THROW(PluginValidatorException(line, &block));
}
