//
// Created by jelle on 3/9/2021.
//

#ifndef PARSEEXCEPTIONS_HPP
#define PARSEEXCEPTIONS_HPP

#include "config/ConfigException.hpp"
#include "config/blocks/RootBlock.hpp"

namespace config {
	class ArgsWithBlockException: public ConfigException {
	protected:
		std::string getTemplate() const throw() {
			return "Block '{KEY}' cannot have any arguments before/after opening bracket";
		}

	public:
		ArgsWithBlockException(const ConfigLine &line): ConfigException(line, 0) {};
	};

	class BlockMissingOpeningException: public ConfigException {
	protected:
		std::string getTemplate() const throw() {
			return "Block '{KEY}' must have opening bracket";
		}

	public:
		BlockMissingOpeningException(const ConfigLine &line): ConfigException(line, 0) {};
	};

	class UnknownKeyException: public ConfigException {
	protected:
		std::string getTemplate() const throw() {
			return "Key '{KEY}' is not a valid config key";
		}

	public:
		UnknownKeyException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
	};

	class InvalidNestedBlockException: public ConfigException {
	protected:
		std::string getTemplate() const throw() {
			return "Block '{KEY}' may not be inside a '{BLOCK_NAME}' block";
		}

	public:
		InvalidNestedBlockException(const std::string &key, int lineNumber, AConfigBlock *parent): ConfigException(ConfigLine(key, lineNumber), parent) {};
	};
}

#endif //PARSEEXCEPTIONS_HPP
