//
// Created by jelle on 3/30/2021.
//

#include "config/validators/RegexCompiler.hpp"
#include "regex/Regex.hpp"

using namespace config;

RegexCompiler::RegexCompiler(int argnum): AConfigValidator(true), _argnum(argnum) {}

void RegexCompiler::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum)
		ERROR_THROW(RegexCompilerException(line, &block));

	try {
		regex::Regex(line.getArg(_argnum));
	} catch (const std::exception &e) {
		ERROR_THROW(RegexCompilerException(line, &block));
	}
}
