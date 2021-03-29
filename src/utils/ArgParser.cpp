//
// Created by jelle on 3/29/2021.
//

#include "utils/ArgParser.hpp"

using namespace utils;

ArgParser::ArgParser(): _isVerbose(false), _isColor(false), _configPath() {}

ArgParser::ArgParser(int argc, char *argv[]): _isVerbose(false), _isColor(false), _configPath() {
	bool readConfig = false;
	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		if (readConfig) {
			_configPath = arg;
			readConfig = false;
			continue;
		}
		if (arg == "-v" || arg == "--verbose")
			_isVerbose = true;
		else if (arg == "-c" || arg == "--color")
			_isColor = true;
		else if (arg == "-f" || arg == "--file")
			readConfig = true;
		else
			throw UnrecognizedArgumentException();
	}

	if (_configPath.empty())
		throw MissingConfigFileException();
}

bool ArgParser::verbosePrint() const {
	return _isVerbose;
}

bool ArgParser::colorPrint() const {
	return _isColor;
}

const std::string &ArgParser::configFile() const {
	return _configPath;
}

std::string ArgParser::printUsage() {
	return std::string(
		"Usage: not-apache [OPTIONS]\n\n"
  		"Worse than apache, don't use in production!\n\n"
		"Options:\n"
  		"  -f,  --file\t\tSpecify Configuration file to use (REQUIRED)\n"
  		"  -v,  --verbose\tTurn on verbose/debug logging\n"
  		"  -c,  --color\t\tTurn on color logging\n"
	);
}
