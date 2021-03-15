//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"

using namespace NotApache;

HTTPParser::ParseState HTTPParser::parse(HTTPClient &client) {
	(void)client;
	return READY_FOR_WRITE;
}


int		HTTPParser::parseBody(std::string line) {
	_body = line;
	return OK;
}

int		HTTPParser::parseHeaders(std::string line) {
	std::vector<std::string> headers = ft::split(line, "\r\n");

	for (size_t i = 0; i < headers.size(); ++i) {
		std::string fieldName = headers[i].substr(0, headers[i].find_first_of(":"));
		for (size_t i = 0; fieldName[i]; ++i)
			fieldName[i] = std::toupper(fieldName[i]);
		if (_headerMap.find(fieldName) == _headerMap.end())
			continue ; // Header not handled
		else
			_headers[_headerMap.find(fieldName)->second] = headers[i].substr(headers[i].find_first_not_of(" ", fieldName.length()+1));
			// ^ Set header ^
	}
	return OK;
}


int		HTTPParser::parseRequestLine(std::string reqLine) {
    std::vector<std::string> parts = ft::split(reqLine, " ");

	if (_methodMap.find(parts[0]) == _methodMap.end()) {
		std::cout << "Invalid method" << std::endl;
		return ERROR;
	}
	// Set Method
	_method = _methodMap.find(parts[0])->second;

	// Set URI
	_uri = parts[1];

	// Set version
	size_t pos = reqLine.find("HTTP/") + 5;
	_version.first = reqLine[pos] - '0';
	_version.second = reqLine[pos+2] - '0';

	return OK;
}

int		HTTPParser::parseRequest(std::string request) {
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield

	EOR = request.find("\r\n");
	if (parseRequestLine(request.substr(0, EOR)))
		return ERROR;
	EOR += 2;

	EOH = request.find("\r\n\r\n", EOR);
	if (EOH == std::string::npos) {
		std::cout << "Request only has request line" << std::endl;
		return OK;
	}
	else if (parseHeaders(request.substr(EOR, EOH-EOR))) 
		return ERROR;
	EOH += 4;

	if (_headers.find(CONTENT_LENGTH) != _headers.end())
		parseBody(request.substr(EOH, ft::stoi(_headers[CONTENT_LENGTH])));
	return OK;
}
