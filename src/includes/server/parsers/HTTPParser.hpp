//
// Created by jelle on 3/3/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/parsers/AParser.hpp"

namespace NotApache {

	class HTTPParser: public AParser {
	public:
		HTTPParser();

		AParser::formatState	formatCheck(Client &client) const;
	};

}

#endif //HTTPPARSER_HPP
