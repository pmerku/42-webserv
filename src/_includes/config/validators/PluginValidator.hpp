//
// Created by jelle on 3/30/2021.
//

#ifndef PLUGINVALIDATOR_HPP
#define PLUGINVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"
#include <vector>

namespace config {

	class PluginValidator : public AConfigValidator {
	private:
		int 									_argnum;
		static const std::vector<std::string>	_plugins;

	public:
		PluginValidator(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class PluginValidatorException: public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "Plugin in {KEY} is not recognized";
			}

		public:
			PluginValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //PLUGINVALIDATOR_HPP
