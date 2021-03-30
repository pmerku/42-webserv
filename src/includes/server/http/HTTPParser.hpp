//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/http/HTTPClient.hpp"
#include "utils/atoi.hpp"
#include "utils/stoi.hpp"
#include "utils/stoh.hpp"
#include "utils/split.hpp"
#include "utils/toUpper.hpp"
#include "utils/countSpaces.hpp"
#include "utils/CreateMap.hpp"

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

		enum MaxSize {
			MAX_HEADER = 8000,
			MAX_BODY = 8000,
			MAX_REQUEST = 24000
		};

		static ParseState		parse(HTTPClient &client);
		static ParseState		parseRequest(HTTPClientRequest& req);
		static ParseState		parseHeaders(HTTPClientRequest& req, std::string rawRequest);
		static ParseState		parseRequestLine(HTTPClientRequest& req, std::string rawRequest);
		static ParseState		parseBody(HTTPClientRequest& req, std::string rawRequest);
		static ParseState		parseChunkedBody(HTTPClientRequest& req, std::string rawRequest);
	
		static const std::map<std::string, e_method>			methodMap_StoE;
		static const std::map<e_method, std::string>			methodMap_EtoS;

	private:
		static const std::string 								allowedURIChars;
	};
}

#endif //HTTPPARSER_HPP
