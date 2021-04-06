//
// Created by jelle on 3/30/2021.
//

#include "config/validators/StartsWithValidator.hpp"

using namespace config;

StartsWithValidator::StartsWithValidator(int argnum, char c, bool reverse): AConfigValidator(true), _argnum(argnum), _c(c), _reverse(reverse) {}

void StartsWithValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum) {
		if (!_reverse)
			ERROR_THROW(StartsWithValidatorException(line, &block, _c));
		ERROR_THROW(StartsWithValidatorReverseException(line, &block, _c));
	}

	const std::string &str = line.getArg(_argnum);
	// must start with
	if (!_reverse) {
		if (str.length() == 0 || str[0] != _c)
			ERROR_THROW(StartsWithValidatorException(line, &block, _c));
		return;
	}

	// cannot start with
	if (str.length() > 0 && str[0] == _c)
		ERROR_THROW(StartsWithValidatorReverseException(line, &block, _c));
}
