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
	public:
		enum ParseState {
			READY_FOR_WRITE,
			ERROR,
			UNFINISHED,
			OK
		};

		static ParseState		parse(HTTPClient &client);
		static ParseState		parseRequest(HTTPClientRequest& _R);
		static ParseState		parseHeaders(HTTPClientRequest& _R, std::string rawRequest);
		static ParseState		parseRequestLine(HTTPClientRequest& _R, std::string rawRequest);
		static ParseState		parseBody(HTTPClientRequest& _R, std::string rawRequest);
		static ParseState		parseChunkedBody(HTTPClientRequest& _R, std::string rawRequest);
	};
}

#endif //HTTPPARSER_HPP
