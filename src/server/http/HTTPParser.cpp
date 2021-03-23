//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"

#include <sstream>

namespace NotApache
{
	std::string methodAsString(const e_method& in) {
		switch (in) {
			case INVALID:
				return "INVALID";
			case GET:
				return "GET";
			case HEAD:
				return "HEAD";
			case POST:
				return "POST";
			case PUT:
				return "PUT";
			case DELETE:
				return "DELETE";
			case CONNECT:
				return "CONNECT";
			case OPTIONS:
				return "OPTIONS";
			case TRACE:
				return "TRACE";
		}
	}

	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x) {
		o	<< "==REQUEST=="								<< std::endl
			<< "Method: "	<< methodAsString(x._method)	<< std::endl
			<< "URI: "		<< x._uri						<< std::endl;
			if (x._headers.size()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<std::string, std::string>::iterator it = x._headers.begin(); it != x._headers.end(); ++it)
					std::cout << "Header: [" << it->first << ": " << it->second << "]" << std::endl;
			}
			else
				std::cout << "-NO HEADERS-" << std::endl;
			if (x._body.length()) {
				std::cout << "Body length: " << x._body.length() << std::endl;
				std::cout << std::endl << "-BODY-" << std::endl << x._body << std::endl;
			}
			else
				std::cout << std::endl << "-NO BODY-" << std::endl;
		return o;
	}
}

using namespace NotApache;

std::map<std::string, e_method> HTTPParser::_methodMap;

HTTPParser::HTTPParser() {
	_methodMap["INVALID"] = INVALID;
	_methodMap["GET"] = GET;
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
	_methodMap["DELETE"] = DELETE;
	_methodMap["CONNECT"] = CONNECT;
	_methodMap["OPTIONS"] = OPTIONS;
	_methodMap["TRACE"] = TRACE;
}

HTTPParser::ParseState		 HTTPParser::parse(HTTPClient& client) {
	if (client.data.request._rawRequest.length() > MAX_REQUEST)	{
		globalLogger.logItem(logger::INFO, "Content-length exceeds max body size");
		client.data.request._statusCode = 400;
		return ERROR;
	}
	if (client.data.request._isChunked)
		return (parseChunkedBody(client.data.request, client.data.request._rawRequest));
	else
		return (parseRequest(client.data.request));
}

HTTPParser::ParseState		HTTPParser::parseChunkedBody(HTTPClientRequest& _R, std::string rawRequest) {
	size_t SOB = 0; // Start Of Body
	size_t EOB = 0; // End Of Body

	// Check for terminating character
	SOB = rawRequest.find_first_of("\r\n");
	if (SOB == std::string::npos)
	{
		globalLogger.logItem(logger::INFO, "No terminating character in body");
		_R._statusCode = 400;
		return ERROR;
	}

	// Get chunksize
	std::string size = rawRequest.substr(0, SOB);
	if (size[0] != '0' && size[1] != 'x')
	{
		globalLogger.logItem(logger::INFO, "Failed to parse chunksize");
		_R._statusCode = 400;
		return ERROR;
	}
	size_t chunkSize = utils::stoh(size.substr(2));
	if (_R._body.length() + chunkSize > MAX_BODY) {
		globalLogger.logItem(logger::INFO, "Content-length exceeds max body size");
		_R._statusCode = 400;
		return ERROR;
	}
	SOB+=2;

	EOB = rawRequest.rfind("\r\n0\r\n\r\n");
	// Check if chunksize matches body
	if (rawRequest.rfind("\r\n")-SOB != chunkSize && EOB-SOB != chunkSize) {
		globalLogger.logItem(logger::INFO, "Chunk size invalid");
		_R._statusCode = 400;
		return ERROR;
	}

	// Append body
	_R._body += rawRequest.substr(SOB, chunkSize);

	// Check if body is complete
	if (EOB != std::string::npos) // 0x0?
	{
		globalLogger.logItem(logger::INFO, "Succesfully parsed chunked body");
		return READY_FOR_WRITE;
	}
	else
	{
		globalLogger.logItem(logger::INFO, "UNFINISHED");
		_R._rawRequest.clear();
		return UNFINISHED;
	}
}

HTTPParser::ParseState		HTTPParser::parseBody(HTTPClientRequest& _R, std::string rawRequest) {
	size_t contentLength = utils::stoi(_R._headers["CONTENT-LENGTH"]);
	if (rawRequest.length() < contentLength)
		return UNFINISHED;
	if (contentLength > MAX_BODY) {
		_R._statusCode = 400;
		globalLogger.logItem(logger::ERROR, "Content-length exceeds max body size");
		return ERROR;
	}
	_R._body += rawRequest.substr(0, contentLength);
	return READY_FOR_WRITE;
}

HTTPParser::ParseState		HTTPParser::parseHeaders(HTTPClientRequest& _R, std::string rawRequest) {
	if (rawRequest.length() > MAX_HEADER) {
		_R._statusCode = 431;
		globalLogger.logItem(logger::ERROR, "Headers exceed max header size");
		return ERROR;
	}
	
	std::vector<std::string> headers = utils::split(rawRequest, "\r\n");
	
	for (size_t i = 0; i < headers.size(); ++i) {
		//Check for colon
		if (headers[i].find(":") == std::string::npos) {
			_R._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "no \":\" in header line");
			return ERROR;
		}
		//Check for spaces in field-name
		if (utils::countSpaces(headers[i].substr(0, headers[i].find_first_of(":")))) {
			_R._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "Spaces in field-name" );
			return ERROR;
		}

		std::string fieldName = headers[i].substr(0, headers[i].find_first_of(":"));
		for (size_t i = 0; fieldName[i]; ++i)
			fieldName[i] = utils::toUpper(fieldName[i]);

		_R._headers[fieldName] = headers[i].substr(headers[i].find_first_not_of(" ", fieldName.length()+1), headers[i].find_last_not_of(" "));
		// ^ Set header ^
	}

	std::map<std::string, std::string>::iterator it = _R._headers.find("TRANSFER-ENCODING");
	if (it != _R._headers.end() && it->second.find("chunked") != std::string::npos)
		_R._isChunked = true;

	it = _R._headers.find("CONTENT-LENGTH");
	if (it != _R._headers.end()) {
		if (_R._isChunked) {
			_R._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "Headers Transfer-encoding + Content-length not allowed");
			return ERROR;
		}
	}
	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequestLine(HTTPClientRequest& _R, std::string rawRequest) {
	// CHECKING GLOBAL FORMAT
	size_t spaces = utils::countSpaces(rawRequest);
	std::vector<std::string> parts = utils::split(rawRequest, " ");
	if (spaces != 2 || parts.size() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid request line");
		_R._statusCode = 400;
		return ERROR;
	}

	// Check Method
	if (_methodMap.find(parts[0]) == _methodMap.end()) {
		_R._statusCode = 501; // 501 (Not Implemented)
		globalLogger.logItem(logger::ERROR, "Invalid method");
		return ERROR;
	}
	// Set Method
	_R._method = _methodMap.find(parts[0])->second;

	// CHECK URI
	if (parts[1][0] != '/') {
		_R._statusCode = 401;
		globalLogger.logItem(logger::ERROR, "Invalid URI");
		return ERROR;
	}

	std::string allowedURIChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~!#$&'()*+,/:;=?@[]");
	for (size_t i = 0; i < parts[1].size(); ++i) {
		if (allowedURIChars.find(parts[1][i]) == std::string::npos) {
			_R._statusCode = 401;
			globalLogger.logItem(logger::ERROR, "Invalid char in URI");
			return ERROR;
		}
	}

	// Set URI
	_R._uri = parts[1];

	// CHECK PROTOCOL
	if (parts[2] != "HTTP/1.1") {
		globalLogger.logItem(logger::ERROR, "Invalid protocol");
		return ERROR;
	}
	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequest(HTTPClientRequest& _R) {
	std::string request = _R._rawRequest;
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield

	EOH = request.find("\r\n\r\n");
	if (EOH == std::string::npos) {
		globalLogger.logItem(logger::DEBUG, "UNFINISHED: no EOF (\"\\r\\n\\r\\n\") in request");
		return UNFINISHED;
	}
	EOR = request.find("\r\n");
	if (parseRequestLine(_R, request.substr(0, EOR)) == ERROR)
		return ERROR;

	if (EOR == EOH) {
		globalLogger.logItem(logger::DEBUG, "Request line only");
		return READY_FOR_WRITE;
	}
	else if (parseHeaders(_R, request.substr(EOR, EOH-EOR)) == ERROR) 
		return ERROR;

	if (_R._isChunked)
		return (parseChunkedBody(_R, request.substr(EOH+4)));
	else if (_R._headers.find("CONTENT-LENGTH") != _R._headers.end())
		return (parseBody(_R, request.substr(EOH+4)));
	else
		return READY_FOR_WRITE;
}
