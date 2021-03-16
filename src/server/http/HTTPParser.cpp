//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"

#include <sstream>

using namespace NotApache;

HTTPParser::ParseState		 HTTPParser::parse(HTTPClient& client) {
	_R = &client.data.request;
	
	if (_R->_chunked)
		return (parseChunkedBody(_R->_rawRequest));
	else
		return (parseRequest(_R->_rawRequest));
}

HTTPParser::ParseState		HTTPParser::parseChunkedBody(std::string request) {
	size_t SOB = 0; // Start Of Body

	SOB = request.find_first_of("\r\n");
	size_t chunkSize = ft::stoh(request.substr(0, SOB));
	SOB+=2;
	if (chunkSize > request.length()-SOB)
		return ERROR;

	_R->_body += request.substr(SOB, chunkSize);
	_R->_bodySize += chunkSize;

	if (request.rfind("\r\n0\r\n\r\n") != std::string::npos) {
		globalLogger.logItem(logger::INFO, "Succesfully parsed chunked body");
		return READY_FOR_WRITE;
	}
	else {
		globalLogger.logItem(logger::INFO, "UNFINISHED");
		_R->_rawRequest.clear();
		return UNFINISHED;
	}
}

HTTPParser::ParseState		HTTPParser::parseBody(std::string request) {
	size_t EOB = 0; // End Of Body

	_R->_bodySize = ft::stoi(_R->_headers[CONTENT_LENGTH]);
	EOB = request.rfind("\r\n");
	if (EOB == std::string::npos)
		return ERROR;
	request = request.substr(0, EOB);

	if (request.length() != _R->_bodySize) {
		globalLogger.logItem(logger::ERROR, "Body size doesn't match");
		return ERROR;
	}
	_R->_body += request;
	return READY_FOR_WRITE;
}

HTTPParser::ParseState		HTTPParser::parseHeaders(std::string line) {
	std::vector<std::string> headers = ft::split(line, "\r\n");

	for (size_t i = 0; i < headers.size(); ++i) {
		if (headers[i].find(":") == std::string::npos) {
			globalLogger.logItem(logger::ERROR, "no \":\" in header line");
			return ERROR;
		}
		//Check for spaces in field-name
		if (ft::countWS(headers[i].substr(0, headers[i].find_first_of(":")))) {
			globalLogger.logItem(logger::ERROR, "Spaces in field-name" );
			return ERROR;
		}

		std::string fieldName = headers[i].substr(0, headers[i].find_first_of(":"));
		for (size_t i = 0; fieldName[i]; ++i)
			fieldName[i] = std::toupper(fieldName[i]);
		if (_R->_headerMap.find(fieldName) == _R->_headerMap.end())
			continue ; // Header not handled
		else
			_R->_headers[_R->_headerMap.find(fieldName)->second] = headers[i].substr(headers[i].find_first_not_of(" ", fieldName.length()+1));
			// ^ Set header ^
	}

	std::map<e_headers, std::string>::iterator it = _R->_headers.find(TRANSFER_ENCODING);
	if (it != _R->_headers.end() && it->second == "chunked")
		_R->_chunked = true;

	it = _R->_headers.find(CONTENT_LENGTH);
	if (it != _R->_headers.end()) {
		if (_R->_chunked) {
			globalLogger.logItem(logger::ERROR, "Headers Transfer-encoding + Content-length not allowed");
			return ERROR;
		}
		_R->_bodySize = ft::stoi(it->second);
	}
	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequestLine(std::string reqLine) {
	// CHECKING GLOBAL FORMAT
	size_t spaces = ft::countWS(reqLine);
    std::vector<std::string> parts = ft::split(reqLine, " ");
	if (spaces != 2 || parts.size() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid request line");
		return ERROR;
	}

	// Check Method
	if (_R->_methodMap.find(parts[0]) == _R->_methodMap.end()) {
		globalLogger.logItem(logger::ERROR, "Invalid method");
		return ERROR;
	}
	// Set Method
	_R->_method = _R->_methodMap.find(parts[0])->second;

	// CHECK URI
	if (parts[1][0] != '/') {
		globalLogger.logItem(logger::ERROR, "Invalid uri");
		return ERROR;
	}
	// Set URI
	_R->_uri = parts[1];

	// CHECK PROTOCOL
	size_t pos = parts[2].find("HTTP/");
	if (pos == std::string::npos) {
		globalLogger.logItem(logger::ERROR, "Invalid protocol");
		return ERROR;
	}
	
	// CHECK VERSION
	pos += 5;
	if (!std::isdigit(parts[2][pos]) && parts[2][pos+1] != '.' && !std::isdigit(parts[2][pos+2])){
		globalLogger.logItem(logger::ERROR, "Invalid version format");
		return ERROR;
	}

	// Set version
	_R->_version.first = parts[2][pos] - '0';
	_R->_version.second = parts[2][pos+2] - '0';

	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequest(std::string request) {
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield

	EOR = request.find("\r\n");
	if (EOR == std::string::npos) {
		globalLogger.logItem(logger::INFO, "UNFINISHED");
		globalLogger.logItem(logger::DEBUG, "No \"\\r\\n\" in request");
		return UNFINISHED; //or error?
	}
	if (parseRequestLine(request.substr(0, EOR)) == ERROR)
		return ERROR;
	EOR += 2;

	EOH = request.find("\r\n\r\n", EOR);

	if (EOH == std::string::npos) {
		globalLogger.logItem(logger::INFO, "Request only has request line");
		return READY_FOR_WRITE;
	}
	else if (parseHeaders(request.substr(EOR, EOH-EOR)) == ERROR) 
		return ERROR;
	EOH += 4;

	if (_R->_chunked)
		return (parseChunkedBody(request.substr(EOH)));
	else if (_R->_headers.find(CONTENT_LENGTH) != _R->_headers.end())
		return (parseBody(request.substr(EOH)));
	else
		return READY_FOR_WRITE;
}
