//
// Created by jelle on 3/8/2021.
//

#include "config/validators/ArgumentLength.hpp"

using namespace config;

ArgumentLength::ArgumentLength(int exact): AConfigValidator(false), _min(exact), _max(exact) {}
ArgumentLength::ArgumentLength(int min, int max): AConfigValidator(false), _min(min), _max(max) {}

void	ArgumentLength::test(const ConfigLine &line, const AConfigBlock &block) const {
	(void)block;
	if (!(line.getArgLength() >= (ConfigLine::arg_size)_min && line.getArgLength() <= (ConfigLine::arg_size)_max))
		throw ArgumentLengthException();
}
