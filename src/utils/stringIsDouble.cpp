#include "utils/stringIsDouble.hpp"
#include "utils/isDigit.hpp"

bool		utils::stringIsDouble(const std::string& str)
{
	int point = 0;
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '.')
			++point;
		else if (!utils::isDigit(str[i]) || point > 1)
			return false;
	}
	return true;
}
