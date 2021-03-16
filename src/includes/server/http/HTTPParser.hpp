//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/http/HTTPClient.hpp"
#include "server/http/ft_utils.hpp"
#include <iostream>

namespace NotApache {
	class HTTPParser {
		HTTPClientRequest*		_R;
	
	public:
		enum ParseState {
			READY_FOR_WRITE,
			ERROR,
			UNFINISHED,
			OK
		};

		ParseState		parse(HTTPClient &client);
		ParseState		parseRequest(std::string request);
		ParseState		parseHeaders(std::string line);
		ParseState		parseRequestLine(std::string reqLine);
		ParseState		parseBody(std::string line);
		ParseState		parseChunkedBody(std::string request);
	};
}

#endif //HTTPPARSER_HPP
