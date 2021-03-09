//
// Created by jelle on 3/9/2021.
//

#include "config/validators/IntValidator.hpp"
#include <cstdlib>

using namespace config;

IntValidator::IntValidator(int argnum, int min, int max): AConfigValidator(false), _min(min), _max(max), _argnum(argnum) {}

// TODO parse int
void IntValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	const std::string &numStr = line.getArg(_argnum);

	// validate characters
	for (std::string::size_type i = 0; i < numStr.length(); ++i) {
		if (numStr[i] < '0' || numStr[i] > '9') throw IntValidatorException(line, &block);
	}

	// check range
	int num = std::atoi(line.getArg(_argnum).c_str());
	if (num < _min || num > _max) throw IntValidatorRangeException(line, &block);
}
