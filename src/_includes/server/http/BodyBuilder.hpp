//
// Created by mramadan on 22/03/2021.
//

#ifndef BODYBUILDER_HPP
#define BODYBUILDER_HPP

#include <string>
#include <map>
#include <vector>

namespace NotApache {

	class BodyBuilder {
	private:
		std::string _body;

		static const std::string _endLine;
		static const std::string _indent;
		static const std::string _tab;

	public:
		BodyBuilder();

		BodyBuilder& line(const std::string& type, const std::string& value);
		BodyBuilder& line(const std::string& type, size_t value);
		BodyBuilder& time(const std::string& txt, time_t time);
		BodyBuilder& mode(const std::string& type, mode_t mode);

		const std::string numberFormat(size_t number);
		const std::string stringFormat(const std::string& str);
		const std::string typeFormat(const std::string& str);
	
		
		const std::string& build();
	};

} // namespace NotApache

#endif //BODYBUILDER_HPP
