//
// Created by jelle on 3/8/2021.
//

#include "config/ConfigParser.hpp"
#include "config/ConfigLine.hpp"
#include "config/AConfigBlock.hpp"
#include "config/blocks/RootBlock.hpp"
#include "config/blocks/ServerBlock.hpp"
#include "config/blocks/RouteBlock.hpp"
#include <unistd.h>
#include <fcntl.h>


// TODO remove logs
#include <iostream>

using namespace config;

const std::string	ConfigParser::possibleBlocks[] = { "server", "route", "" };

void	ConfigParser::parseFile(const std::string &path) const {
	char		buf[1024];
	::ssize_t	len = sizeof(buf)-1;
	std::string	out;

	// read file data into out
	int fd = ::open(path.c_str(), O_RDONLY, 0);
	if (fd == -1) throw FailedToOpenException();
	while (true) {
		::ssize_t ret = ::read(fd, buf, len);
		// handle errors
		if (ret == -1) {
			close(fd);
			throw FailedToReadException();
		}
		// add new data to string
		if (ret > 0) {
			buf[ret] = 0;
			out += buf;
		}
		// get out of loop if enough is read
		if (ret != len) break;
	}
	close(fd);

	// parse all lines
	AConfigBlock	*rootBlock = new RootBlock(ConfigLine("root {"));
	AConfigBlock	*currentBlock = rootBlock;
	int blockDepth = 0;
	std::string::size_type	i = 0;
	do {
		// get line
		std::string::size_type	newline = out.find('\n', i);
		std::string::size_type	endOfCharacters = newline;
		if (out[newline-1] == '\r') endOfCharacters--;

		// remove comments
		std::string	line = out.substr(i, endOfCharacters-i);
		std::string::size_type	hashtag = line.find('#');
		line = line.substr(0, hashtag);

		// parse line
		try {
			ConfigLine parsedLine(line);
			if (isAllowedBlock(parsedLine.getKey())) {
				AConfigBlock	*newBlock;
				if (parsedLine.getKey() == "server") {
					newBlock = new ServerBlock(parsedLine, currentBlock);
				} else if (parsedLine.getKey() == "route") {
					newBlock = new RouteBlock(parsedLine, currentBlock);
				} else {
					newBlock = new RootBlock(parsedLine, currentBlock);
				}
				try {
					currentBlock->addBlock(newBlock);
				} catch (std::exception &e) {
					delete newBlock;
					throw e;
				}
				currentBlock = newBlock;
				blockDepth++;
			}
			else if (parsedLine.getKey().length() > 0 && parsedLine.getKey()[0] == '}') {
				AConfigBlock::validateEndBlock(parsedLine);
				currentBlock = currentBlock->getParent();
				if (currentBlock == 0) {
					delete rootBlock;
					throw UnbalancedBracketsException();
				}
				blockDepth--;
			}
			else currentBlock->addLine(parsedLine);
		} catch (const ConfigLine::MissingKeyException &e) {}

		// prepare for next iteration
		i = newline == std::string::npos ? newline : newline+1;
	} while (i != std::string::npos);

	if (blockDepth != 0) {
		delete rootBlock;
		throw UnbalancedBracketsException();
	}

	rootBlock->runPostValidators();
	rootBlock->print();
	return; // TODO return block;
//	std::cout << "--------------" << std::endl;
//			std::cout << "Line num: " << parsedLine.getLineNumber() << std::endl;
//			std::cout << "Arg len: " << parsedLine.getArgLength() << std::endl;
//			std::cout << "line: '" << parsedLine.getKey() << "'" ;
//			for (ConfigLine::arg_size i2 = 0; i2 < parsedLine.getArgLength(); ++i2) {
//				std::cout << " '" << parsedLine.getArg(i2) << "'";
//			}
//			std::cout << std::endl;
}

bool ConfigParser::isAllowedBlock(const std::string &key) const {
	int	i = 0;
	while (ConfigParser::possibleBlocks[i].length() > 0) {
		if (key == ConfigParser::possibleBlocks[i]) return true;
		i++;
	}
	return false;
}
