//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/RouteBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/RequiredKey.hpp"
#include "config/validators/Unique.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	RouteBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("location", ConfigValidatorListBuilder() // TODO location validator (start with slash) + regex validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("allowed_methods", ConfigValidatorListBuilder() // TODO method validator
		  .add(new ArgumentLength(0, 9))
		  .add(new Unique())
		  .build())
		.addKey("root", ConfigValidatorListBuilder() // TODO directory validator + mutally exclusive validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("directory_listing", ConfigValidatorListBuilder() // TODO boolean validator + default to false
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("index", ConfigValidatorListBuilder() // TODO filename validator + default to index.html
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("cgi", ConfigValidatorListBuilder() // TODO file validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("save_uploads", ConfigValidatorListBuilder() // TODO directory validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("use_plugin", ConfigValidatorListBuilder() // TODO plugin validator
		  .add(new ArgumentLength(1))
		  .build())
		.addKey("proxy_url", ConfigValidatorListBuilder() // TODO mutally exclusive validator + url validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
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

RouteBlock::RouteBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent) {}

const std::string RouteBlock::getType() const {
	return "route";
}

const AConfigBlock::validatorListType &RouteBlock::getBlockValidators() const {
	return _blockValidators;
}

void	RouteBlock::cleanup() {
	for (validatorsMapType::const_iterator i = _validators.begin(); i != _validators.end(); ++i) {
		for (validatorListType::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			delete *j;
		}
	}
	for (validatorListType::const_iterator i = _blockValidators.begin(); i != _blockValidators.end(); ++i) {
		delete *i;
	}
}
