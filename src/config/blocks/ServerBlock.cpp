//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/ServerBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/Unique.hpp"
#include "config/validators/RequiredKey.hpp"
#include "config/validators/IntValidator.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	ServerBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("host", ConfigValidatorListBuilder() // TODO validate ip
			  .add(new ArgumentLength(1))
			  .add(new Unique())
			  .build())
	  	.addKey("port", ConfigValidatorListBuilder()
			  .add(new ArgumentLength(1))
			  .add(new Unique())
			  .add(new IntValidator(0, 1, 65535))
			  .build())
		.addKey("server_name", ConfigValidatorListBuilder() // TODO default to something
			.add(new ArgumentLength(1))
			.add(new Unique())
			.build())
		.addKey("error_page", ConfigValidatorListBuilder() // TODO file validator
			.add(new ArgumentLength(2))
			.add(new IntValidator(0, 400, 600))
			.build())
		.addKey("body_limit", ConfigValidatorListBuilder() // TODO default to -1
				.add(new ArgumentLength(1))
				.add(new IntValidator(0, 0, 600))
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
