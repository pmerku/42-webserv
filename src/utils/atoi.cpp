#include "utils/atoi.hpp"

size_t		utils::atoi(const char* str)
{
	size_t ret = 0;
	for (size_t i = 0; str[i]; ++i) {
		if (!utils::isDigit(str[i]))
			return (0);
		ret = ret * 10 + (str[i] - '0');
	}
	return ret;
}
