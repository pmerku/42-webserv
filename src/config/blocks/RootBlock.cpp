//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/RootBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/IntValidator.hpp"
#include <cstdlib>

using namespace config;

const AConfigBlock::validatorsMapType	RootBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("use_workers", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new IntValidator(0, 1, 10000))
		  .build())
	  	.build();

const AConfigBlock::validatorListType 	RootBlock::_blockValidators =
		ConfigValidatorListBuilder() // TODO validate server duplicates
		.build();

const std::string 						RootBlock::_allowedBlocks[] = { "server", "" };

const AConfigBlock::validatorsMapType	&RootBlock::getValidators() const {
	return RootBlock::_validators;
}

const std::string						*RootBlock::getAllowedBlocks() const {
	return RootBlock::_allowedBlocks;
}

RootBlock::RootBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent) {}

std::string RootBlock::getType() const {
	return "root";
}

const AConfigBlock::validatorListType &RootBlock::getBlockValidators() const {
	return _blockValidators;
}

void	RootBlock::cleanup() {
	for (validatorsMapType::const_iterator i = _validators.begin(); i != _validators.end(); ++i) {
		for (validatorListType::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			delete *j;
		}
	}
	for (validatorListType::const_iterator i = _blockValidators.begin(); i != _blockValidators.end(); ++i) {
		delete *i;
	}
}

// TODO parse int
void	RootBlock::parseData() {
	_workerCount = -1;
	if (hasKey("use_workers"))
		_workerCount = std::atoi(getKey("use_workers")->getArg(0).c_str());
	for (std::vector<AConfigBlock*>::iterator i = _blocks.begin(); i != _blocks.end(); ++i) {
		if (dynamic_cast<ServerBlock*>(*i))
			_serverBlocks.push_back(reinterpret_cast<ServerBlock*>(*i));
		(*i)->parseData();
	}
	_isParsed = true;
}

int	RootBlock::getWorkerCount() const {
	throwNotParsed();
	return _workerCount;
}

const std::vector<ServerBlock *>	&RootBlock::getServerBlocks() const {
	throwNotParsed();
	return _serverBlocks;
}
