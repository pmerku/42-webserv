//
// Created by jelle on 3/8/2021.
//

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <list>
#include <exception>

namespace config {

	class ConfigParser {
	public:
		static const std::string	possibleBlocks[];
		bool	isAllowedBlock(const std::string	&key) const;
		void parseFile(const std::string &path) const;

		class UnbalancedBracketsException: public std::exception {
		public:
			const char * what() const throw() {
				return "ConfigParser: Unbalanced brackets in configuration";
			}
		};

		class FailedToOpenException: public std::exception {
		public:
			const char * what() const throw() {
				return "ConfigParser: Configuration file failed to open";
			}
		};

		class FailedToReadException: public std::exception {
		public:
			const char * what() const throw() {
				return "ConfigParser: Failed to read from configuration file";
			}
		};
	};

}

#endif //CONFIGPARSER_HPP
