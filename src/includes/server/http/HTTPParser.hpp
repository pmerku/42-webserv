//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/http/HTTPParseData.hpp"
#include "server/http/HTTPClient.hpp"
#include "utils/atoi.hpp"
#include "utils/stoi.hpp"
#include "utils/stoh.hpp"
#include "utils/split.hpp"
#include "utils/toUpper.hpp"
#include "utils/countSpaces.hpp"
#include "utils/CreateMap.hpp"
#include "utils/DataList.hpp"
#include "utils/Uri.hpp"

#include <iostream>

namespace NotApache {

	class HTTPParser {
	public:
		enum ParseState {
			READY_FOR_WRITE,
			UNFINISHED
		};


	private:
		static const int maxHeaderSize;
		enum ParseReturn {
			ERROR,
			FINISHED,
			OK
		};

		static ParseReturn		parseRequestLine(HTTPParseData &data, const std::string &line);
		static ParseReturn		parseResponseLine(HTTPParseData &data, const std::string &line);

		static ParseReturn		parseHeaders(HTTPParseData &data, const std::string &headers, HTTPClient *client);

	public:

		static ParseState		parse(HTTPClient &client);
		static ParseState		parse(HTTPParseData &data, HTTPClient *client = 0);
	
		static const std::map<std::string, e_method>			methodMap_StoE;
		static const std::map<e_method, std::string>			methodMap_EtoS;

	private:
		static const std::string 								allowedURIChars;
	};
}

#endif //HTTPPARSER_HPP
