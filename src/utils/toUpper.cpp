#include "utils/toUpper.hpp"

int		utils::toUpper(int c)
{
	if (c >= 'a' && c <= 'z')
		c = c - 32;
	return (c);
}

void	utils::toUpper(std::string &str) {
	for (std::string::size_type i = 0; i < str.length(); ++i)
		str[i] = utils::toUpper(str[i]);
}
