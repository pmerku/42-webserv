//
// Created by jelle on 3/8/2021.
//

#include "config/validators/Unique.hpp"
#include "config/AConfigBlock.hpp"

using namespace config;

Unique::Unique(): AConfigValidator(false) {}

void	Unique::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (block.hasKey(line.getKey()))
		throw UniqueException(line, &block);
}
