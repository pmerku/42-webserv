//
// Created by jelle on 4/1/2021.
//

#ifndef HTTPPARSEDATA_HPP
#define HTTPPARSEDATA_HPP

#include "utils/DataList.hpp"
#include "utils/Uri.hpp"
#include <iostream>
#include <map>
#include <string>

namespace NotApache {

	enum e_method {
		INVALID,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE
	};

	class HTTPParseData {
	public:
		enum HTTPParseType {
			CGI_RESPONSE,
			RESPONSE,
			REQUEST
		};
		utils::DataList						data;
	    utils::DataList						chunkedData;

	private:
		utils::DataList::DataListIterator	_pos;
		HTTPParseType 						_type;
		bool								_gotHeaders;
		bool								_gotFirstLine;
		bool 								_gotBody;
	    bool 								_gotTrailHeaders;

	public:
		HTTPParseData(HTTPParseType type = REQUEST);

		bool								isChunked;
		e_method							method;
		utils::Uri							uri;
		std::map<std::string, std::string>	headers;
		int 								bodyLength;
		int 								statusCode;
		std::string 						reasonPhrase;

		int									parseStatusCode;

		friend class HTTPParser;
	};

}

std::ostream& 	operator<<(std::ostream& o, NotApache::HTTPParseData& x);

#endif //HTTPPARSEDATA_HPP
