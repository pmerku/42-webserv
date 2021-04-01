//
// Created by martyparty on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/http/HTTPParseData.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"

namespace NotApache
{
	std::ostream& operator<<(std::ostream& o, HTTPParseData& x) {
		o	<< "==REQUEST=="													<< std::endl
			<< "Method: "	<< HTTPParser::methodMap_EtoS.find(x.method)->second 	<< std::endl
			<< "URI: "		<< x.uri.getFull()										<< std::endl;

			if (!x.headers.empty()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<std::string, std::string>::iterator it = x.headers.begin(); it != x.headers.end(); ++it)
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

HTTPParser::ParseReturn		HTTPParser::parseRequestLine(HTTPParseData &data, const std::string &line) {
	// check if spaces count in line is correct
	std::vector<std::string> parts = utils::split(line, " ");
	if (utils::countSpaces(line) != 2 || parts.size() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid request line");
		data.parseStatusCode = 400;
		return ERROR;
	}

	// check if method is known
	if (methodMap_StoE.find(parts[0]) == methodMap_StoE.end()) {
		globalLogger.logItem(logger::ERROR, "Unknown method");
		data.parseStatusCode = 501;
		return ERROR;
	}
	// set known method
	data.method = methodMap_StoE.find(parts[0])->second;

	// check if URI is valid
	if (parts[1][0] != '/') {
		globalLogger.logItem(logger::ERROR, "URI is malformed");
		data.parseStatusCode = 400;
		return ERROR;
	}

	if (parts[1].find_first_not_of(allowedURIChars) != std::string::npos) {
		globalLogger.logItem(logger::ERROR, "Invalid character in URI");
		data.parseStatusCode = 400;
		return ERROR;
	}
	// set URI
	data.uri = parts[1];

	if (parts[2] != "HTTP/1.1") {
		globalLogger.logItem(logger::ERROR, "HTTP protocol not supported");
		data.parseStatusCode = 505;
		return ERROR;
	}
	return OK;
}

HTTPParser::ParseReturn		HTTPParser::parseResponseLine(HTTPParseData &data, const std::string &line) {
	// check if spaces count in line is correct
	std::vector<std::string> parts = utils::split(line, " ");
	if (utils::countSpaces(line) != 2 || parts.size() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid request line");
		data.parseStatusCode = 400;
		return ERROR;
	}

	if (parts[0] != "HTTP/1.1") {
		globalLogger.logItem(logger::ERROR, "HTTP protocol not supported");
		data.parseStatusCode = 505;
		return ERROR;
	}

	// parse status code (ignore reason-phrase)
	if (parts[1].length() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid status code");
		data.parseStatusCode = 400;
		return ERROR;
	}
	for (std::string::size_type i = 0; i < 3; ++i) {
		if (!utils::isDigit(parts[1][i])) {
			globalLogger.logItem(logger::ERROR, "Invalid status code");
			data.parseStatusCode = 400;
			return ERROR;
		}
	}
	data.statusCode = utils::stoi(parts[1]);
	return OK;
}

HTTPParser::ParseReturn		HTTPParser::parseHeaders(HTTPParseData &data, const std::string &headers, HTTPClient *client) {
	std::vector<std::string> headersArray = utils::split(headers, "\r\n");
	for (std::vector<std::string>::iterator it = headersArray.begin(); it != headersArray.end(); ++it) {
		std::string::size_type colonPos = it->find(':');
		if (colonPos == std::string::npos) {
			globalLogger.logItem(logger::ERROR, "no \":\" in header field");
			data.parseStatusCode = 400;
			return ERROR;
		}

		// key parsing
		std::string	key = it->substr(0, colonPos);
		utils::toUpper(key);
		if (key.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~!#$&'*+") != std::string::npos) {
			globalLogger.logItem(logger::ERROR, "Invalid character in header field name");
			data.parseStatusCode = 400;
			return ERROR;
		}

		// add to map
		std::string	value = it->substr(colonPos+1);
		value = value.substr(value.find_first_not_of(' '), value.find_last_not_of(' '));
		data.headers[key] = value;
		// TODO ignore on cgi & parse stricter (transfer-encoding: gzip, chunked should fail)
		if (key == "TRANSFER-ENCODING" && value.find("chunked") != std::string::npos)
			data._isChunked = true;
	}

	// at least one header
	if (data.headers.empty()) {
		globalLogger.logItem(logger::ERROR, "No headers in parsed data");
		data.parseStatusCode = 400;
		return ERROR;
	}

	// content-length and transfer encoding may not exist on same request (ignore on cgi
	if (data._type != HTTPParseData::CGI_RESPONSE) {
		std::map<std::string, std::string>::iterator transferIt = data.headers.find("TRANSFER-ENCODING");
		std::map<std::string, std::string>::iterator contentIt = data.headers.find("CONTENT-LENGTH");
		if (transferIt != data.headers.end() && data._isChunked && contentIt != data.headers.end()) {
			globalLogger.logItem(logger::ERROR, "Headers Transfer-encoding + Content-length not allowed");
			data.parseStatusCode = 400;
			return ERROR;
		}
	}

	// host header needs to exist on requests
	if (data._type == HTTPParseData::REQUEST) {
		int contentLength = 0;
		std::map<std::string,std::string>::iterator hostIt = data.headers.find("HOST");
		if (hostIt == data.headers.end()) {
			globalLogger.logItem(logger::ERROR, "Missing Host header");
			data.parseStatusCode = 400;
			return ERROR;
		}

		std::map<std::string,std::string>::iterator contentLengthIt = data.headers.find("CONTENT-LENGTH");
		if (contentLengthIt != data.headers.end())
			contentLength = utils::stoi(contentLengthIt->second);
		config::ServerBlock *server = NotApache::configuration->findServerBlock(hostIt->second, client->getPort(), client->getHost());
		if (server == 0) {
			globalLogger.logItem(logger::ERROR, "No matching server block");
			data.parseStatusCode = 500;
			return ERROR;
		}
		if (server->getBodyLimit() != -1 && ( contentLength == -1 || contentLength > server->getBodyLimit()) ) {
			globalLogger.logItem(logger::ERROR, "Body too large");
			data.parseStatusCode = 413;
			return ERROR;
		}
	}

	return OK;
}

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
