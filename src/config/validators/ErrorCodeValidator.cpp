//
// Created by jelle on 3/30/2021.
//

#include "config/validators/ErrorCodeValidator.hpp"
#include "utils/stoi.hpp"
#include "utils/CreateVector.hpp"

using namespace config;

const std::vector<int>	ErrorCodeValidator::_errorCodes = utils::CreateVector<int>
        (400)
		(404)
		(500);

ErrorCodeValidator::ErrorCodeValidator(int argnum): AConfigValidator(false), _argnum(argnum) {}

void ErrorCodeValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum)
		throw ErrorCodeValidatorException(line, &block);
	int code = utils::stoi(line.getArg(_argnum));
	for (std::vector<int>::const_iterator it = _errorCodes.begin(); it != _errorCodes.end(); ++it) {
		if (*it == code)
			return; // found valid code
	}
	// not found, invalid
	throw ErrorCodeValidatorException(line, &block);
}
