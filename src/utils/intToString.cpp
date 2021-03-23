//
// Created by pmerku on 3/19/2021.
//

#include "utils/intToString.hpp"

std::string	utils::intToString(int x) {
	if (x < 0)
		return std::string("-") + utils::intToString(-x);
	if (x < 10)
		return std::string(1, x + '0');
	return utils::intToString(x / 10) + utils::intToString(x % 10);
}
