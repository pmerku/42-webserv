//
// Created by jelle on 3/10/2021.
//

#include "config/validators/BooleanValidator.hpp"

using namespace config;

BooleanValidator::BooleanValidator(int argnum) : AConfigValidator(true), _argnum(argnum) {}

void BooleanValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArg(_argnum) != "true" && line.getArg(_argnum) != "false")
		ERROR_THROW(BooleanValidatorException(line, &block));
}
