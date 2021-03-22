//
// Created by pmerku on 22/03/2021.
//

#include <cstdlib>
#include <string>
#include "utils/strdup.hpp"

char *utils::strdup(const std::string &str) {
	size_t len = str.length();
	char *mem;

	if (!(mem = static_cast<char *>(malloc(sizeof(char) * (len + 1)))))
		return NULL;
	size_t i = 0;
	for (; i < len; i++)
		mem[i] = str[i];
	mem[i] = '\0';
	return mem;
}
