//
// Created by jelle on 3/9/2021.
//

#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <exception>
#include <string>
#include "config/AConfigBlock.hpp"

namespace config {

	class ConfigException: public std::exception {
	private:
		static bool replaceInString(std::string& str, const std::string& from, const std::string& to);

	protected:
		const ConfigLine	_line;
		const AConfigBlock	*_block;

		virtual std::string getTemplate() const throw() {
			return "{KEY} is invalid inside of {BLOCK_NAME}";
		}

	public:
		ConfigException(const ConfigLine &line, const AConfigBlock *block);

		virtual ~ConfigException() throw();

		const ConfigLine	&getLine() const;

		const char * what() const throw() {
			return "Configuration is invalid";
		}

		std::string	prettyPrint() const throw();
	};

}

#endif //CONFIGEXCEPTION_HPP
