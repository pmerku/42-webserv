//
// Created by jelle on 3/8/2021.
//

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <list>
#include <exception>
#include "log/Loggable.hpp"
#include "config/ConfigException.hpp"

namespace config {

class ConfigParser: public log::ILoggable {
	public:
		static const std::string	possibleBlocks[];
		bool	isAllowedBlock(const std::string &key) const;
		void parseFile(const std::string &path) const;

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
			const char * getTemplate() const throw() {
				return "Unbalanced brackets detected in file";
			}

		public:
			UnbalancedBracketsException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //CONFIGPARSER_HPP
