#include "utils/stoi.hpp"
#include <climits>

int		utils::stoi(const std::string& str)
{
	size_t ret = 0;
	for (size_t i = 0; str[i]; ++i) {
		if (!utils::isDigit(str[i]))
			return (0);
		ret = ret * 10 + (str[i] - '0');
		if (ret > INT_MAX)
			return -1;
	}
	return ret;
}
