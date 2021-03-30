//
// Created by jelle on 3/29/2021.
//

#ifndef HTTPMIMETYPES_HPP
#define HTTPMIMETYPES_HPP

#include <string>
#include <map>

namespace MimeTypes {
	extern const std::map<std::string, std::string> types;

	std::string getMimeType(const std::string &ext);
}

#endif //HTTPMIMETYPES_HPP
