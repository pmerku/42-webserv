//
// Created by jelle on 3/8/2021.
//

#include "config/validators/RequiredKey.hpp"
#include "config/AConfigBlock.hpp"

using namespace config;

RequiredKey::RequiredKey(const std::string &key): _key(key) {}

void	RequiredKey::test(const ConfigLine &line, const AConfigBlock &block) const {
	(void)line;
	if (!block.hasKey(_key))
		throw RequiredKeyException(_key, &block);
}
