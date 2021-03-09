//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/RootBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	RootBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("testing", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(2))
		  .build())
	  	.build();

const AConfigBlock::validatorListType 	RootBlock::_blockValidators =
		ConfigValidatorListBuilder()
		.build();

const std::string 						RootBlock::_allowedBlocks[] = { "server", "" };

const AConfigBlock::validatorsMapType	&RootBlock::getValidators() const {
	return RootBlock::_validators;
}

const std::string						*RootBlock::getAllowedBlocks() const {
	return RootBlock::_allowedBlocks;
}

RootBlock::RootBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent) {}

const std::string RootBlock::getType() const {
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
