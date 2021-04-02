//
// Created by jelle on 4/1/2021.
//

#include "server/http/HTTPParseData.hpp"
#include <iostream>

using namespace NotApache;

HTTPParseData::HTTPParseData(HTTPParseData::HTTPParseType type):
	data(),
	_pos(data.beginList()),
	_isChunked(false),
	_type(type),
	_gotHeaders(false),
	_gotFirstLine(false),
	_gotBody(false),
	method(),
	bodyLength(0),
	statusCode(0),
	parseStatusCode(200)
{
	if (_type == CGI_RESPONSE)
		_gotFirstLine = true;
}

std::ostream& operator<<(std::ostream& o, HTTPParseData& x) {
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
	if (!x.data.empty()) {
		o	<< "Body length: " << x.data.size() 						<< std::endl << std::endl
			 << "-BODY-" 												<< std::endl
			 << x.data.substring(x.data.beginList(), x.data.endList()) 	<< std::endl;
	}
	else
		o << std::endl << "-NO BODY-" 									<< std::endl;
	return o;
}
