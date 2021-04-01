//
// Created by jelle on 4/1/2021.
//

#include "server/http/HTTPParseData.hpp"

using namespace NotApache;

HTTPParseData::HTTPParseData(HTTPParseData::HTTPParseType type):
	data(),
	_pos(data.beginList()),
	_isChunked(false),
	_type(type),
	_gotHeaders(false),
	_gotFirstLine(false),
	method(),
	statusCode(),
	parseStatusCode(200)
{
	if (_type == CGI_RESPONSE)
		_gotFirstLine = true;
}
