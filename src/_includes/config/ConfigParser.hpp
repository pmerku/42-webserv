//
// Created by jelle on 3/8/2021.
//

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <list>
#include <exception>
#include <config/blocks/RootBlock.hpp>
#include "log/Loggable.hpp"
#include "log/LogItem.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class ConfigParser: public logger::ILoggable {
	private:
		static std::string _parseLine(const std::string &file, unsigned long i, unsigned long *newlinePos);
		static std::string _readFile(const std::string &path);
		static AConfigBlock *_makeBlockFromLine(const ConfigLine &line, int lineCount, AConfigBlock *currentBlock);

	public:
		static const std::string	possibleBlocks[];
		bool	isAllowedBlock(const std::string &key) const;
		RootBlock * parseFile(const std::string &path) const;

		class FailedToOpenException: public std::exception {
		public:
			const char * what() const throw() {
				return "Configuration file failed to open";
			}
		};

		class FailedToReadException: public std::exception {
		public:
			const char * what() const throw() {
				return "Failed to read from configuration file";
			}
		};

		class UnbalancedBracketsException: public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "Unbalanced brackets detected in file";
			}

		public:
			UnbalancedBracketsException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};

};

}

#endif //CONFIGPARSER_HPP
