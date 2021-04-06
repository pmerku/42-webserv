//
// Created by jelle on 4/1/2021.
//

#include "server/http/HTTPParseData.hpp"
#include <iostream>

using namespace NotApache;

HTTPParseData::HTTPParseData(HTTPParseData::HTTPParseType type):
		data(),
		chunkedData(),
		_pos(data.beginList()),
		_isCGI(false),
		_type(type),
		_gotHeaders(false),
		_gotFirstLine(false),
		_gotBody(false),
		_gotTrailHeaders(false),
		isChunked(false),
		method(),
		bodyLength(0),
		statusCode(0),
		parseStatusCode(200)
{
	if (_type == CGI_RESPONSE)
		_gotFirstLine = true;
}

std::ostream& operator<<(std::ostream& o, NotApache::HTTPParseData& x) {
	o	<< "==REQUEST=="					<< std::endl
		 << "Method: "	<< x.method 		<< std::endl
		 << "URI: "		<< x.uri.getFull()	<< std::endl;

	if (!x.headers.empty()) {
		o << std::endl << "-HEADERS-" << std::endl;
		for (std::map<std::string, std::string>::iterator it = x.headers.begin(); it != x.headers.end(); ++it)
			o << "Header: [" << it->first << ": " << it->second << "]" 	<< std::endl;
	}
	else
		o	<< "-NO HEADERS-" 											<< std::endl;
	utils::DataList &body = x.data;
	if (x.isChunked)
		body = x.chunkedData;
	if (!body.empty()) {
		o << "Body length: " << body.size() << std::endl << std::endl
		  << "-BODY-" << std::endl
		  << body.substring(body.beginList(), body.endList()) << std::endl;
	}
	else
		o << std::endl << "-NO BODY-" 									<< std::endl;
	return o;
}
