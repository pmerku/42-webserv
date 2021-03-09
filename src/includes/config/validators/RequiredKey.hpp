//
// Created by jelle on 3/8/2021.
//

#ifndef REQUIREDKEY_HPP
#define REQUIREDKEY_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class RequiredKey: public AConfigBlockValidator {
	private:
		const std::string	_key;

	public:
		RequiredKey(const std::string &key);

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class RequiredKeyException: public ConfigException {
		protected:
			const char * getTemplate() const throw() {
				return "Block {BLOCK_NAME} is missing {KEY}";
			}

		public:
			RequiredKeyException(const std::string	&key, const AConfigBlock *block): ConfigException(ConfigLine(key, block->getLineNumber()), block) {};
		};
	};

}

#endif //REQUIREDKEY_HPP
