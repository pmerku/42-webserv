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
		bool				_requiresOther;

	public:
		RequiredKey(const std::string &key, bool requiresOther = false);

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class RequiredKeyException: public ConfigException {
		private:
			std::string	_other;

		protected:
			std::string getTemplate() const throw() {
				if (_other.empty())
					return "Block {BLOCK_NAME} is missing {KEY}";
				return std::string("Block {BLOCK_NAME} is missing {KEY} (required by ") + _other + ")";
			}

		public:
			RequiredKeyException(const std::string	&key, const AConfigBlock *block, const std::string &other): ConfigException(ConfigLine(key, block->getLineNumber()), block), _other(other) {};
			RequiredKeyException(const std::string	&key, const AConfigBlock *block): ConfigException(ConfigLine(key, block->getLineNumber()), block), _other() {};
			~RequiredKeyException() throw() {};
		};
	};

}

#endif //REQUIREDKEY_HPP
