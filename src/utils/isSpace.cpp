//
// Created by jelle on 3/9/2021.
//

#include "utils/isSpace.hpp"

bool	utils::isSpace(const char c) {
	return (c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\n' || c == '\v');
}
