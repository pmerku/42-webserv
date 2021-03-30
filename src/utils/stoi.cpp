#include "utils/stoi.hpp"

size_t		utils::stoi(const std::string& str)
{
	size_t ret = 0;
	for (size_t i = 0; str[i]; ++i) {
		if (!utils::isDigit(str[i]))
			return (0);
		ret = ret * 10 + (str[i] - '0');
	}
	return ret;
}
