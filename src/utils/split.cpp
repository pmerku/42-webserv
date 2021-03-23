#include "utils/split.hpp"

std::vector<std::string>		utils::split(const std::string& str, const std::string& del)
{
	size_t start = 0, end = 0;
	std::vector<std::string> array;


	while (end != str.npos)
	{
		start = str.find_first_not_of(del, end);
		end = str.find_first_of(del, start);
		if (end != std::string::npos || start != std::string::npos)
			array.push_back(str.substr(start, end - start));
	}
	return array;
}
