//
// Created by jelle on 3/29/2021.
//

#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <string>
#include <exception>

namespace utils {

	class ArgParser {
	private:
		bool		 _isVerbose;
		bool 		_isColor;
		std::string _configPath;

	public:
		ArgParser();
		ArgParser(int argc, char *argv[]);

		static std::string printUsage();

		bool				verbosePrint() const;
		bool 				colorPrint() const;
		const std::string 	&configFile() const;

		class ArgParserException: public std::exception {
		public:
			const char * what() const throw() {
				return "Invalid arguments";
			}
		};

		class MissingConfigFileException: public ArgParserException {
		public:
			const char * what() const throw() {
				return "Missing config file in execution arguments";
			}
		};

		class UnrecognizedArgumentException: public ArgParserException {
		public:
			const char * what() const throw() {
				return "Unrecognized argument found in execution arguments";
			}
		};

	};

}

#endif //ARGPARSER_HPP
