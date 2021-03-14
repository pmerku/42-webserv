//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/http/HTTPClient.hpp"

namespace NotApache {

	class HTTPParser {
	public:
		enum ParseState {
			UNFINISHED,
			READY_FOR_WRITE
		};

		ParseState	parse(HTTPClient &client);
	};

}

#endif //HTTPPARSER_HPP
