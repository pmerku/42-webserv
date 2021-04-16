//
// Created by mramadan on 22/03/2021.
//

#ifndef JSONBUILDER_HPP
#define JSONBUILDER_HPP

#include <sys/stat.h>
#include <string>
#include <map>

namespace NotApache {

	class JsonBuilder {
	private:
		std::string _body;
		std::map<std::string, std::string> _bodyMap;

		static const std::string _endLine;
		static const std::string _indent;
		static const std::string _tab;

	public:
		JsonBuilder();

		JsonBuilder& addLine(const std::string& type, const std::string& value);
		JsonBuilder& addLine(const std::string& type, size_t value);
		JsonBuilder& time(const std::string& txt, time_t time);
		JsonBuilder& mode(const std::string& type, mode_t mode);
		

		char getMode(mode_t value, int mask, char c);
		const std::string numberFormat(size_t number);
		const std::string stringFormat(const std::string& str);
		const std::string typeFormat(const std::string& str);
	
		
		const std::string& build();
	};

} // namespace NotApache

#endif //JSONBUILDER_HPP
