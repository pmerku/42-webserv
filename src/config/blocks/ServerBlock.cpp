//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/ServerBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/Unique.hpp"
#include "config/validators/RequiredKey.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	ServerBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("host", ConfigValidatorListBuilder()
			  .add(new ArgumentLength(1))
			  .add(new Unique())
			  .build())
	  	.addKey("port", ConfigValidatorListBuilder()
			  .add(new ArgumentLength(1))
			  .add(new Unique())
			  .build())
		.addKey("server_name", ConfigValidatorListBuilder()
			.add(new ArgumentLength(1))
			.add(new Unique())
			.build())
		.addKey("error_page", ConfigValidatorListBuilder()
			.add(new ArgumentLength(2))
			.build())
	  	.build();

const AConfigBlock::validatorListType 	ServerBlock::_blockValidators =
		ConfigValidatorListBuilder()
		.add(new RequiredKey("host"))
		.add(new RequiredKey("port"))
		.build();

const std::string 						ServerBlock::_allowedBlocks[] = { "route", "" };

const AConfigBlock::validatorsMapType	&ServerBlock::getValidators() const {
	return ServerBlock::_validators;
}

const std::string						*ServerBlock::getAllowedBlocks() const {
	return ServerBlock::_allowedBlocks;
}

ServerBlock::ServerBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent) {}

const std::string ServerBlock::getType() const {
	return "server";
}

const AConfigBlock::validatorListType &ServerBlock::getBlockValidators() const {
	return _blockValidators;
}

void	ServerBlock::cleanup() {
	for (validatorsMapType::const_iterator i = _validators.begin(); i != _validators.end(); ++i) {
		for (validatorListType::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			delete *j;
		}
	}
	for (validatorListType::const_iterator i = _blockValidators.begin(); i != _blockValidators.end(); ++i) {
		delete *i;
	}
}
