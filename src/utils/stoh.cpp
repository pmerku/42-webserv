#include "utils/stoh.hpp"

size_t		utils::stoh(const std::string& str)
{
	char c;
	size_t i = 0;
	size_t ret = 0;
	size_t abc = 10;
	
	while (str[i] == ' ')
		i++;
	while (i < str.length())
	{
		if (utils::isDigit(str[i]))
			ret = ret * 16 + (str[i] - '0');
		else {
			c = utils::toUpper(str[i]);
			while (c != 'A')
			{
				--c;
				++abc;
			}
			ret = ret * 16 + abc;
		}
		i++;
	}
	return ret;
}
