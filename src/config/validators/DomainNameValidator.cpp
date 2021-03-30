//
// Created by jelle on 3/30/2021.
//

#include "config/validators/DomainNameValidator.hpp"
#include "utils/isDigit.hpp"

using namespace config;

DomainNameValidator::DomainNameValidator(int argnum): AConfigValidator(true), _argnum(argnum) {}

void DomainNameValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum)
		throw DomainNameValidatorException(line, &block);
	const std::string &str = line.getArg(_argnum);
	for (std::string::size_type i = 0; i < str.length(); i++) {
		// IF not digit AND not alpha AND not dash AND not dot THEN throw
		if (!utils::isDigit(str[i]) && !(str[i] >= 'A' && str[i] <= 'Z') && !(str[i] >= 'a' && str[i] <= 'z') && str[i] != '-' && str[i] != '.')
			throw DomainNameValidatorException(line, &block);
	}
}
