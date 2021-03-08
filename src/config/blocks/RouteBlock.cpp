//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/RouteBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/RequiredKey.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	RouteBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("test", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(0))
		  .build())
	  	.build();

const AConfigBlock::validatorListType 	RouteBlock::_blockValidators =
		ConfigValidatorListBuilder()
		.add(new RequiredKey("location"))
		.build();

const std::string 						RouteBlock::_allowedBlocks[] = { "" };

const AConfigBlock::validatorsMapType	&RouteBlock::getValidators() const {
	return RouteBlock::_validators;
}

const std::string						*RouteBlock::getAllowedBlocks() const {
	return RouteBlock::_allowedBlocks;
}

RouteBlock::RouteBlock(const ConfigLine &line, AConfigBlock *parent): AConfigBlock(line, parent) {}

const std::string RouteBlock::getType() const {
	return "route";
}

const AConfigBlock::validatorListType &RouteBlock::getBlockValidators() const {
	return _blockValidators;
}

RouteBlock::~RouteBlock() {
	for (validatorsMapType::const_iterator i = _validators.begin(); i != _validators.end(); ++i) {
		for (validatorListType::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			delete *j;
		}
	}
	for (validatorListType::const_iterator i = _blockValidators.begin(); i != _blockValidators.end(); ++i) {
		delete *i;
	}
}
