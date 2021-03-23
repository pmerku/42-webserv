#include "utils/isDigit.hpp"

int		utils::isDigit(int c)
{
	if (c >= '0' && c <='9')
		return (1);
	else
		return (0);
}
