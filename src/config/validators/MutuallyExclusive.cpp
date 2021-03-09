//
// Created by jelle on 3/9/2021.
//

#include "config/validators/MutuallyExclusive.hpp"

using namespace config;

MutuallyExclusive::MutuallyExclusive(const std::string& one, const std::string& two) : AConfigValidator(true), _one(one), _two(two) {}

void MutuallyExclusive::test(const ConfigLine &line, const AConfigBlock &block) const {
	(void)line;
	if (block.hasKey(_one) && block.hasKey(_two))
		throw MutuallyExclusiveException(*(block.getKey(_one)), &block);
	if (!block.hasKey(_one) && !block.hasKey(_two))
		throw MutuallyExclusiveMissingException(ConfigLine(_one, block.getLineNumber()), &block);
}
