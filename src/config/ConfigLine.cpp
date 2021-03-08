//
// Created by jelle on 3/8/2021.
//

#include "config/ConfigLine.hpp"
#include <cctype>

using namespace config;

// TODO not use std::isspace
ConfigLine::ConfigLine(const std::string &line, int lineNum): _lineNum(lineNum) {
	if (line.length() == 0) throw MissingKeyException();
	for (std::string::size_type i = 0, args = 0, len = line.length(); i < len; ++args) {
		// skip initial whitespace
		for (; i < len; ++i) if (!std::isspace(line[i])) break;
		if (i == len) { // reached end of line
			if (args == 0) throw MissingKeyException(); // throw if no key
			else break; // stop parsing
		}

		// create arg until whitespace
		std::string::size_type	begin = i;
		for (; i < len; ++i) if (std::isspace(line[i])) break;
		std::string out(line, begin, i - begin);
		if (args == 0) _key = out; // make key if first part
		else _args.push_back(out); // otherwise add to args
	}
	_argLen = _args.size();
}

ConfigLine::arg_size	ConfigLine::getArgLength() const {
	return _argLen;
}

std::string ConfigLine::getArg(arg_size i) const {
	return _args[i];
}

std::string ConfigLine::getKey() const {
	return _key;
}

int ConfigLine::getLineNumber() const {
	return _lineNum;
}
