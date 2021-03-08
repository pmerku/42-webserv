//
// Created by jelle on 3/8/2021.
//

#ifndef CONFIGLINE_HPP
#define CONFIGLINE_HPP

#include <vector>
#include <string>
#include <exception>

namespace config {

	class ConfigLine {
	public:
		typedef	std::vector<std::string>::size_type	arg_size;

	private:
		std::string					_key;
		int							_lineNum;
		std::vector<std::string>	_args;
		arg_size					_argLen;

	public:
		ConfigLine(const std::string &line, int lineNum = 0);

		arg_size		getArgLength() const;
		std::string		getArg(arg_size i) const;
		std::string		getKey() const;
		int 			getLineNumber() const;

		class MissingKeyException: public std::exception {
		public:
			const char * what() const throw() {
				return "ConfigLine: Line doesn't contain a key";
			}
		};
	};
}

#endif //CONFIGLINE_HPP
