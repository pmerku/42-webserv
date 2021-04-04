//
// Created by martyparty on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"
#include <algorithm>
#include "utils/intToString.hpp"

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
	// TODO cgi headers (status must parse to status code)
	// TODO cgi headers ignore X-GI-* headers
	// check if header field name has correct format
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
		// TODO parse stricter (transfer-encoding: gzip, chunked should fail)
		if (!data._isCGI && key == "TRANSFER-ENCODING" && value.find("chunked") != std::string::npos)
			data.isChunked = true;
	}

	// at least one header
	if (data.headers.empty()) {
		globalLogger.logItem(logger::ERROR, "No headers in parsed data");
		data.parseStatusCode = 400;
		return ERROR;
	}

	// content-length and transfer encoding may not exist on same request (ignore on cgi)
	if (data._type != HTTPParseData::CGI_RESPONSE) {
		std::map<std::string, std::string>::iterator transferIt = data.headers.find("TRANSFER-ENCODING");
		std::map<std::string, std::string>::iterator contentIt = data.headers.find("CONTENT-LENGTH");
		if (transferIt != data.headers.end() && data.isChunked && contentIt != data.headers.end()) {
			globalLogger.logItem(logger::ERROR, "Headers Transfer-encoding + Content-length not allowed");
			data.parseStatusCode = 400;
			return ERROR;
		}
	}

	// host header needs to exist on requests
	if (data._type == HTTPParseData::REQUEST) {
		std::map<std::string,std::string>::iterator hostIt = data.headers.find("HOST");
		if (hostIt == data.headers.end()) {
			globalLogger.logItem(logger::ERROR, "Missing Host header");
			data.parseStatusCode = 400;
			return ERROR;
		}

		std::map<std::string,std::string>::iterator contentLengthIt = data.headers.find("CONTENT-LENGTH");
		if (contentLengthIt != data.headers.end())
			data.bodyLength = utils::stoi(contentLengthIt->second);
		config::ServerBlock *server = NotApache::configuration->findServerBlock(hostIt->second, client->getPort(), client->getHost());
		if (server == 0) {
			globalLogger.logItem(logger::ERROR, "No matching server block");
			data.parseStatusCode = 500;
			return ERROR;
		}
		if (server->getBodyLimit() != -1 && ( data.bodyLength == -1 || data.bodyLength > server->getBodyLimit()) ) {
			globalLogger.logItem(logger::ERROR, "Body too large");
			data.parseStatusCode = 413;
			return ERROR;
		}
	}

	return OK;
}

HTTPParser::ParseReturn HTTPParser::parseTrailHeaders(HTTPParseData &data, const std::string &headers) {
	// check if header field name has correct format
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
		// ignore already present headers
		data.headers.erase(data.headers.find("TRAILER"));
		if (data.headers.find(key) == data.headers.end())
			data.headers[key] = value;
	}

	return OK;
}

HTTPParser::ParseReturn		HTTPParser::parseBody(HTTPParseData &data, utils::DataList::DataListIterator it) {
	if (data.data.size(it) < static_cast<utils::DataList::size_type>(data.bodyLength))
		return OK; // unfinished
	data.data.resize(it, data.data.endList());
	return FINISHED;
}

HTTPParser::ParseReturn		HTTPParser::parseChunkedBody(HTTPParseData &data, utils::DataList::DataListIterator it) {
	data._pos = it;

	while (true) {
		utils::DataList::DataListIterator sizeEnd = data.data.find("\r\n", data._pos);
		// check if CRLF was transmitted
		if (sizeEnd == data.data.endList()) {
			return OK; // unfinished
		}

		// parse chunk size
		std::string	tilEndl = data.data.substring(data._pos, sizeEnd);
		tilEndl = tilEndl.substr(0, tilEndl.find(';'));
		if (tilEndl.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
			globalLogger.logItem(logger::ERROR, "Invalid characters in chunk size");
			data.parseStatusCode = 400;
			return ERROR;
		}
		size_t chunkSize = utils::stoh(tilEndl);
		std::advance(sizeEnd, 2);
		if (data.data.size(sizeEnd) < chunkSize + 2) {
			return OK; // unfinished
		} else if (chunkSize == 0) {
			if (data.data.find("\r\n", sizeEnd) != data.data.endList())
				data._gotTrailHeaders = true;
			data._pos = sizeEnd;
			return FINISHED;
		}

		utils::DataList::DataListIterator chunkEnd = sizeEnd;

		// move chunkEnd by amount of characters
		std::advance(chunkEnd, chunkSize);

		if (chunkEnd == data.data.endList()) {
			return OK; // unfinished
		}

		// check for CRLF at end of chunk data
		if (data.data.find("\r\n", chunkEnd) != chunkEnd) {
			globalLogger.logItem(logger::ERROR, "No CRLF characters after chunk data");
			data.parseStatusCode = 400; // TODO which code?
			return ERROR;
		}

		// extract chunk data
		data.chunkedData.add(data.data.substring(sizeEnd, chunkEnd).c_str(), chunkSize); // TODO test if this removes \0 chars
		std::advance(chunkEnd, 2); // now past \r\n at end of chunk
		data.data.resize(chunkEnd, data.data.endList());

		data._pos = data.data.beginList(); // set last position of chunk
	}
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

	if (!data._gotBody) {
		utils::DataList::DataListIterator beginOfData = data._pos; // 4. move it var to start of new chunk
		std::advance(beginOfData, 2);

		ParseReturn	ret;
		if (data.isChunked)
			ret = parseChunkedBody(data, beginOfData);
		else
			ret = parseBody(data, beginOfData);

		if (ret == FINISHED) {
			data._gotBody = true;
			std::map<std::string, std::string>::iterator it = data.headers.find("TRANSFER-ENCODING");
			if (it != data.headers.end()) {
				std::string arr = "chunked"; // remove chunked from transfer-encoding header
				for (std::string::size_type i = 0; i < arr.length(); ++i) {
					it->second.erase(std::remove(it->second.begin(), it->second.end(), arr.at(i)), it->second.end());
				}
				data.headers["CONTENT-LENGTH"] = utils::intToString(data.chunkedData.size()); // set content-length header
			}
		}
		else if (ret == ERROR)
			return READY_FOR_WRITE;
		else
			return UNFINISHED;
	}

	if (data.isChunked && !data._gotTrailHeaders) {
		// find header terminator, ends with CRLFCRLF
		utils::DataList::DataListIterator endOfHeaders = data.data.find("\r\n\r\n", data._pos);
		utils::DataList::DataListIterator beginOfHeaders = data._pos;

		// no header terminator found, unfinished
		if (endOfHeaders == data.data.endList())
			return UNFINISHED;
		else if (endOfHeaders == beginOfHeaders) { // no trail headers
			data._gotTrailHeaders = true;
			return READY_FOR_WRITE;
		}

		// max size check
		if (data.data.size(beginOfHeaders, endOfHeaders) > maxHeaderSize - data.headers.size()) {
			globalLogger.logItem(logger::ERROR, "Header field is too large");
			data.parseStatusCode = 431;
			return READY_FOR_WRITE;
		}

		ParseReturn ret = parseTrailHeaders(data, data.data.substring(beginOfHeaders, endOfHeaders));
		if (ret == ERROR)
			return READY_FOR_WRITE;
		data.data.resize(data.data.beginList(), beginOfHeaders);
		data._gotTrailHeaders = true;
	}
	if (data.isChunked)
		data.data.clear();
	return READY_FOR_WRITE;
}
