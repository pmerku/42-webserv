//
// Created by pmerku on 3/19/2021.
//

#include "utils/intToString.hpp"

#include <cstdlib>

//std::string intToString(int x) {
//	if (x < 0)
//		return std::string("-") + intToString(-x);
//	if (x < 10)
//		return std::string(1, x + '0');
//	return intToString(x / 10) + intToString(x % 10);
//}

static size_t intLen(int x) {
	size_t len = (x <= 0) ? 1 : 0;
	while (x) {
		x /= 10;
		len++;
	}
	return len;
}

std::string intToString(int x) {
	size_t len = intLen(x);
	char *str = static_cast<char *>(::malloc(sizeof(char) * (len + 1)));

	if (!str)
		throw "MALLOC"; // TODO cleanup
	if (x < 0)
		str[0] = '-';
	else if (x == 0)
		str[0] = '0';
	else
		x *= -1;
	str[len] = '\0';
	while (len-- && x) {
		str[len - 1] = -(x % 10) + '0';
		x /= 10;
	}
	::free(str);
	return std::string(str);
}
