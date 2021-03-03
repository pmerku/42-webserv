//
// Created by jelle on 3/3/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/AParser.hpp"

namespace NotApache {

	class HTTPParser: public AParser {
	public:
		HTTPParser();

		AParser::parseState	parse(Client &client) const;
	};

}

#endif //HTTPPARSER_HPP
