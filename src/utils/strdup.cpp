//
// Created by pmerku on 22/03/2021.
//

#include <cstdlib>
#include <string>
#include "utils/strdup.hpp"

char *utils::strdup(const std::string &str) {
	size_t len = str.length();
	char *mem = new char[len + 1]();
	for (size_t i = 0; i < len; i++)
		mem[i] = str[i];
	return mem;
}
