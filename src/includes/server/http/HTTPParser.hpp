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
		enum e_ret {
			OK,
			ERROR
		};
		enum ParseState {
			UNFINISHED,
			READY_FOR_WRITE
		};

		ParseState				parse(HTTPClient &client);
		int						parseRequest(std::string request);
		int						parseHeaders(std::string line);
		int						parseRequestLine(std::string reqLine);
		int						parseBody(std::string line);
	};

}

#endif //HTTPPARSER_HPP
