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

		BodyBuilder& fileName(const std::string& name);
		BodyBuilder& fileSize(const std::string& size);
		BodyBuilder& fileType(const std::string& type);

		const std::string& build();
	};

} // namespace NotApache

#endif //BODYBUILDER_HPP
