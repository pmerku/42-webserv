//
// Created by jelle on 3/9/2021.
//

#include "utils/isSpace.hpp"

using namespace utils;

bool	isSpace(const char c) {
	return (c == '\r' || c == '\t' || c == '\f' || c == '\n' || c == '\v');
}
