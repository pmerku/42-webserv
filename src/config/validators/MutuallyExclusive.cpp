//
// Created by jelle on 3/9/2021.
//

#include "config/validators/MutuallyExclusive.hpp"
#include "utils/split.hpp"

using namespace config;

MutuallyExclusive::MutuallyExclusive(const std::string& str) : AConfigValidator(true), _list() {
	_list = utils::split(str, ";");
}

void MutuallyExclusive::test(const ConfigLine &line, const AConfigBlock &block) const {
    (void)line;
	bool found = false;
    for (std::vector<std::string>::const_iterator it = _list.begin(); it != _list.end(); ++it) {
        bool hasKey = block.hasKey(*it);
		if (!found && hasKey)
			found = true;
		else if (hasKey) {
		    ERROR_THROW(MutuallyExclusiveException(line, &block, _list));
		}
    }
	if (!found) {
        ERROR_THROW(MutuallyExclusiveMissingException(ConfigLine(_list.front(), block.getLineNumber()), &block, _list));
	}
}
