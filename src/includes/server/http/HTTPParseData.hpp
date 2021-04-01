//
// Created by jelle on 4/1/2021.
//

#ifndef HTTPPARSEDATA_HPP
#define HTTPPARSEDATA_HPP

#include "utils/DataList.hpp"
#include "utils/Uri.hpp"
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

	private:
		utils::DataList::DataListIterator	_pos;
		bool								_isChunked;
		HTTPParseType 						_type;
		bool								_gotHeaders;
		bool								_gotFirstLine;

	public:
		HTTPParseData(HTTPParseType type = REQUEST);

		e_method							method;
		utils::Uri							uri;
		std::map<std::string, std::string>	headers;
		utils::DataList						body;
		int 								statusCode;

		int									parseStatusCode;

		friend class HTTPParser;
	};
}

#endif //HTTPPARSEDATA_HPP
