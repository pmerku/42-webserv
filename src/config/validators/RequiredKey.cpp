//
// Created by jelle on 3/8/2021.
//

#include "config/validators/RequiredKey.hpp"

using namespace config;

RequiredKey::RequiredKey(const std::string &key, bool requiresOther): _key(key), _requiresOther(requiresOther) {}

void	RequiredKey::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (!block.hasKey(_key)) {
		if (_requiresOther)
			ERROR_THROW(RequiredKeyException(_key, &block, line.getKey()));
		ERROR_THROW(RequiredKeyException(_key, &block));
	}
}
