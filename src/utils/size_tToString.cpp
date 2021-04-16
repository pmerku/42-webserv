//
// Created by mramadan on 3/19/2021.
//

#include "utils/size_tToString.hpp"

std::string	utils::size_tToString(size_t x) {
	if (x < 10)
		return std::string(1, x + '0');
	return utils::size_tToString(x / 10) + utils::size_tToString(x % 10);
}
