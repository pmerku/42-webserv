#include "utils/toUpper.hpp"

int		utils::toUpper(int c)
{
	if (c >= 'a' && c <= 'z')
		c = c - 32;
	return (c);
}
