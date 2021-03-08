//
// Created by jelle on 3/8/2021.
//

#include "config/ConfigBlock.hpp"






#include <iostream>


using namespace config;

void ConfigBlock::addLine(const ConfigLine &line) {
	_lines.push_back(line);
}

void ConfigBlock::addBlock(ConfigBlock *block) {
	_blocks.push_back(block);
}

ConfigBlock *ConfigBlock::getParent() const {
	return _parent;
}

void ConfigBlock::runPostValidators() const {

}

ConfigBlock::ConfigBlock(const ConfigLine &line, ConfigBlock *parent): _parent(parent) {
	if (line.getArgLength() != 1)
		throw ArgsWithBlockException();
	else if (line.getArg(0) != "{")
		throw BlockMissingOpeningException();
}

ConfigBlock::~ConfigBlock() {
	for (std::vector<ConfigBlock *>::iterator i = _blocks.begin(); i != _blocks.end(); ++i) {
		delete *i;
	}
}

void ConfigBlock::print(unsigned int spaces) const {
	std::string prefix;
	for (unsigned int i = 0; i < spaces; i++) prefix += " ";
	std::cout << prefix << "{\n";
	spaces+=2;prefix = "";
	for (unsigned int i = 0; i < spaces; i++) prefix += " ";
	for (std::vector<ConfigLine>::const_iterator i = _lines.begin(); i != _lines.end(); ++i) {
		const ConfigLine &line = *i;
		std::cout << prefix << line.getKey();
		for (ConfigLine::arg_size j = 0; j < line.getArgLength(); ++j)
			std::cout << " " << line.getArg(j);
		std::cout << "\n";
	}
	for (std::vector<ConfigBlock*>::const_iterator i = _blocks.begin(); i != _blocks.end(); ++i) {
		(*i)->print(spaces);
	}
	spaces-=2;prefix = "";
	for (unsigned int i = 0; i < spaces; i++) prefix += " ";
	std::cout << prefix << "}" << std::endl;
}

void ConfigBlock::validateEndBlock(const ConfigLine &line) {
	if (line.getArgLength() != 0 || line.getKey() != "}" || line.getKey().length() > 1)
		throw ArgsWithBlockException();
}
