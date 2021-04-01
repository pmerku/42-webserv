//
// Created by martyparty on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"

namespace NotApache
{
	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x) {
		o	<< "==REQUEST=="													<< std::endl
			<< "Method: "	<< HTTPParser::methodMap_EtoS.find(x._method)->second 	<< std::endl
			<< "URI: "		<< x._uri											<< std::endl;

			if (!x.headers.empty()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<std::string, std::string>::iterator it = x._headers.begin(); it != x._headers.end(); ++it)
					o << "Header: [" << it->first << ": " << it->second << "]" 	<< std::endl;
			}
			else
				o	<< "-NO HEADERS-" 											<< std::endl;
//			if (x._body.length()) {
//				o	<< "Body length: " << x._body.length() 						<< std::endl << std::endl
//					<< "-BODY-" 												<< std::endl
//					<< x._body 													<< std::endl;
//			}
//			else
			o << std::endl << "-NO BODY-" 									<< std::endl;
		return o;
	}
}

using namespace NotApache;

const std::string HTTPParser::allowedURIChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~!#$&'()*+,/:;=?@[]";
const int HTTPParser::maxHeaderSize = 8000;

const std::map<std::string, e_method> HTTPParser::methodMap_StoE =
		utils::CreateMap<std::string, e_method>
		("INVALID", INVALID)
		("GET", GET)
		("HEAD", HEAD)
		("POST", POST)
		("PUT", PUT)
		("DELETE", DELETE)
		("CONNECT", CONNECT)
		("OPTIONS", OPTIONS)
		("TRACE", TRACE);

const std::map<e_method, std::string> HTTPParser::methodMap_EtoS =
		utils::CreateMap<e_method, std::string>
		(INVALID, "INVALID")
		(GET, "GET")
		(HEAD, "HEAD")
		(POST, "POST")
		(PUT, "PUT")
		(DELETE, "DELETE")
		(CONNECT, "CONNECT")
		(OPTIONS, "OPTIONS")
		(TRACE, "TRACE");

HTTPParser::ParseState		 HTTPParser::parse(HTTPClient& client) {
	return parse(client.data.request.data, &client);
}

HTTPParser::ParseState		HTTPParser::parse(HTTPParseData &data, HTTPClient *client) {
	if (!data._gotFirstLine) {
		// find status line, ends with CRLF
		utils::DataList::DataListIterator endOfStatus = data.data.find("\r\n");
		// no status line found, unfinished
		if (endOfStatus == data.data.endList())
			return UNFINISHED;

		// max size check
		if (data.data.size(data.data.beginList(), endOfStatus) > maxHeaderSize) {
			globalLogger.logItem(logger::ERROR, "Header field is too large");
			data.parseStatusCode = 431;
			return READY_FOR_WRITE;
		}

		ParseReturn	ret = ERROR;
		if (data._type == HTTPParseData::REQUEST)
			ret = parseRequestLine(data, data.data.substring(data.data.beginList(), endOfStatus));
		else if (data._type == HTTPParseData::RESPONSE)
			ret = parseResponseLine(data, data.data.substring(data.data.beginList(), endOfStatus));

		// if error occurred, start writing error response
		if (ret == ERROR)
			return READY_FOR_WRITE;
		data._pos = endOfStatus; // points to \r\n of statusline
		data._gotFirstLine = true;
	}

	if (!data._gotHeaders) {
		// find header terminator, ends with CRLFCRLF
		utils::DataList::DataListIterator endOfHeaders = data.data.find("\r\n\r\n", data._pos);
		utils::DataList::DataListIterator beginOfHeaders = data._pos;

		// max size check
		std::cout << "Size: " << data.data.size(beginOfHeaders, endOfHeaders) << std::endl;
		if (data.data.size(beginOfHeaders, endOfHeaders) > maxHeaderSize) {
			globalLogger.logItem(logger::ERROR, "Header field is too large");
			data.parseStatusCode = 431;
			return READY_FOR_WRITE;
		}

		// no header terminator found, unfinished
		if (endOfHeaders == data.data.endList())
			return UNFINISHED;

		// parse! if error occurred, start writing error response
		ParseReturn	ret = parseHeaders(data, data.data.substring(beginOfHeaders, endOfHeaders), client);
		if (ret == ERROR)
			return READY_FOR_WRITE;
		std::advance(endOfHeaders, 2);
		data._pos = endOfHeaders; // points to /r/n of empty line
		data._gotHeaders = true;
	}

	// TODO parse body + chunked body
	if (data._gotHeaders) {
		return READY_FOR_WRITE;
	}
	return UNFINISHED;
}
