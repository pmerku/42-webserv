//
// Created by pmerku on 22/03/2021.
//

#include <cstdlib>
#include <string>
#include "utils/strdup.hpp"
#include <cstring>

char *utils::strdup(const std::string &str) {
	size_t len = str.length();
	char *mem = new char[len + 1]();
	std::memcpy(mem, str.c_str(), len);
	return mem;
}
