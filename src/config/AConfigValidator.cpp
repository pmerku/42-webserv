//
// Created by jelle on 3/8/2021.
//

#include "config/AConfigValidator.hpp"

using namespace config;

AConfigValidator::AConfigValidator(bool isPostValidator): _postValidator(isPostValidator) {}

bool AConfigValidator::isPostValidator() const {
	return _postValidator;
}

AConfigValidator::~AConfigValidator() {}

AConfigBlockValidator::AConfigBlockValidator(): AConfigValidator(true) {}
