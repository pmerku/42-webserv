#include "utils/countSpaces.hpp"

size_t		utils::countSpaces(const std::string& str) {
	size_t spaces = 0;
	for (size_t i = 0; str[i]; ++i)
		if (str[i] == ' ')
			++spaces;
	return spaces;
}
