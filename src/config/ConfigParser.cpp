//
// Created by jelle on 3/8/2021.
//

#include "config/ConfigParser.hpp"
#include "config/blocks/RootBlock.hpp"
#include "config/blocks/ServerBlock.hpp"
#include "config/blocks/RouteBlock.hpp"
#include "utils/ErrorThrow.hpp"
#include <unistd.h>
#include <fcntl.h>

using namespace config;

const std::string	ConfigParser::possibleBlocks[] = { "server", "route", "" };

std::string		ConfigParser::_readFile(const std::string &path) {
	char		buf[1024];
	::ssize_t	len = sizeof(buf)-1;
	std::string	out;

	int fd = ::open(path.c_str(), O_RDONLY, 0);
	if (fd == -1) ERROR_THROW(FailedToOpenException());
	while (true) {
		::ssize_t ret = ::read(fd, buf, len);
		// handle errors
		if (ret == -1) {
			::close(fd);
			ERROR_THROW(FailedToReadException());
		}
		// add new data to string
		if (ret > 0) {
			buf[ret] = 0;
			out += buf;
		}
		// get out of loop if enough is read
		if (ret != len) break;
	}
	::close(fd);
	return out;
}

std::string		ConfigParser::_parseLine(const std::string &file, std::string::size_type i, std::string::size_type *newlinePos) {
	// get line
	std::string::size_type	newline = file.find('\n', i);
	std::string::size_type	endOfCharacters = newline;
	if (newline != std::string::npos && newline > 0 && file[newline-1] == '\r') endOfCharacters--;

	// remove comments
	std::string	line = file.substr(i, endOfCharacters-i);
	std::string::size_type	hashtag = line.find('#');
	line = line.substr(0, hashtag);

	*newlinePos = newline;
	return line;
}

AConfigBlock	*ConfigParser::_makeBlockFromLine(const ConfigLine &line, int lineCount, AConfigBlock *currentBlock) {
	if (line.getKey() == "server") {
		return new ServerBlock(line, lineCount, currentBlock);
	} else if (line.getKey() == "route") {
		return new RouteBlock(line, lineCount, currentBlock);
	} else {
		return new RootBlock(line, lineCount, currentBlock);
	}
}

RootBlock		*ConfigParser::parseFile(const std::string &path) const {
	std::string file = _readFile(path);

	// parse all lines
	RootBlock		*rootBlock = new RootBlock(ConfigLine("root {"), 0);
	AConfigBlock	*currentBlock = rootBlock;
	int blockDepth = 0;
	int	lineCount = 1;
	std::string::size_type	i = 0;
	std::string::size_type	newlinePos = 0;
	do {
		std::string line = _parseLine(file, i, &newlinePos);

		// parse line
		try {
			ConfigLine parsedLine(line, lineCount);
			if (isAllowedBlock(parsedLine.getKey())) {
				AConfigBlock	*newBlock = _makeBlockFromLine(parsedLine, lineCount, currentBlock);
				try {
					currentBlock->addBlock(newBlock);
				} catch (std::exception &e) {
					delete newBlock;
					delete rootBlock;
					throw;
				}
				currentBlock = newBlock;
				blockDepth++;
			}
			else if (parsedLine.getKey().length() > 0 && parsedLine.getKey()[0] == '}') {
				AConfigBlock::validateEndBlock(parsedLine);
				currentBlock = currentBlock->getParent();
				if (currentBlock == 0) {
					delete rootBlock;
					ERROR_THROW(UnbalancedBracketsException(parsedLine, 0));
				}
				blockDepth--;
			}
			else currentBlock->addLine(parsedLine);
		} catch (const ConfigLine::MissingKeyException &e) {}
		catch (const ConfigException &e) {
			logItem(e);
			delete rootBlock;
			throw;
		}

		// prepare for next iteration
		i = (newlinePos == std::string::npos) ? newlinePos : newlinePos+1;
		++lineCount;
	} while (i != std::string::npos);

	try {
		if (blockDepth != 0) {
			delete rootBlock;
			ERROR_THROW(UnbalancedBracketsException(ConfigLine("<3", lineCount), 0));
		}
		rootBlock->runPostValidators();
		rootBlock->parseData();
	} catch (const ConfigException &e) {
		logItem(e);
		delete rootBlock;
		throw;
	}
	return rootBlock;
}

bool ConfigParser::isAllowedBlock(const std::string &key) const {
	int	i = 0;
	while (ConfigParser::possibleBlocks[i].length() > 0) {
		if (key == ConfigParser::possibleBlocks[i]) return true;
		i++;
	}
	return false;
}
