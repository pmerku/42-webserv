//
// Created by jelle on 3/9/2021.
//

#include "config/ConfigException.hpp"
#include "config/AConfigBlock.hpp"

using namespace config;

ConfigException::ConfigException(const ConfigLine &line, const AConfigBlock *block): _line(line), _block(block) {}

std::string ConfigException::prettyPrint() const throw() {
	std::string	out = getTemplate();
	replaceInString(out, "{KEY}", _line.getKey());
	if (_block) {
		replaceInString(out, "{BLOCK_NAME}", _block->getType());
		if (_block->getParent()) {
			replaceInString(out, "{PARENT_NAME}", _block->getParent()->getType());
		}
	}
	return out;
}

bool ConfigException::replaceInString(std::string &str, const std::string &from, const std::string &to) {
	std::string::size_type start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

const ConfigLine	&ConfigException::getLine() const {
	return _line;
}

ConfigException::~ConfigException() throw() {}
