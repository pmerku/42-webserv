//
// Created by jelle on 3/8/2021.
//

#include "config/ConfigValidatorBuilder.hpp"

using namespace config;

ConfigValidatorListBuilder &ConfigValidatorListBuilder::add(const AConfigValidator *validator) {
	_validators.push_back(validator);
	return *this;
}

const AConfigBlock::validatorListType &ConfigValidatorListBuilder::build() const {
	return _validators;
}

ConfigValidatorBuilder &
ConfigValidatorBuilder::addKey(const std::string &key, const AConfigBlock::validatorListType validators) {
	_validators[key] = validators;
	return *this;
}

const AConfigBlock::validatorsMapType &ConfigValidatorBuilder::build() const {
	return _validators;
}
