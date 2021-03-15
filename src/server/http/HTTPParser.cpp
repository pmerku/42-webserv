//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"

using namespace NotApache;

HTTPParser::ParseState HTTPParser::parse(HTTPClient &client) {
	_R = &client.data.request;
	parseRequest(_R->_rawRequest);
	return READY_FOR_WRITE;
}


int		HTTPParser::parseBody(std::string line) {
	_R->_body = line;
	return OK;
}


int		HTTPParser::parseHeaders(std::string line) {
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


int		HTTPParser::parseRequestLine(std::string reqLine) {
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


int		HTTPParser::parseRequest(std::string request) {
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield
	size_t EOB = 0; // End Of Body

	EOR = request.find("\r\n");
	if (EOR == std::string::npos) {
		globalLogger.logItem(logger::INFO, "UNFINISHED");
		globalLogger.logItem(logger::DEBUG, "No \"\\r\\n\" in request");
		return UNFINISHED; //or error?
	}
	if (parseRequestLine(request.substr(0, EOR)))
		return ERROR;
	EOR += 2;

	EOH = request.find("\r\n\r\n", EOR);
	if (EOH == std::string::npos) {
		globalLogger.logItem(logger::INFO, "Request only has request line");
		return OK;
	}
	else if (parseHeaders(request.substr(EOR, EOH-EOR))) 
		return ERROR;
	EOH += 4;

	if (_R->_chunked) {
		size_t end = request.rfind("\r\n");
		EOB = request.rfind("\r\n", end-1)+2;
		std::cout << "len: " << request.length() << std::endl;
		std::cout << "EOB: " << EOB << std::endl;
		std::cout << "end: " << end << std::endl;
		std::cout << "substr: " << request.substr(EOB, end-EOB) << std::endl;
		_R->_bodySize = ft::stoi(request.substr(EOB, end-EOB));
		if (_R->_bodySize != 0) {
			globalLogger.logItem(logger::INFO, "UNFINISHED");
			//ret = UNFINISHED;
		}
	}

	if (_R->_headers.find(CONTENT_LENGTH) != _R->_headers.end())
		parseBody(request.substr(EOH, ft::stoi(_R->_headers[CONTENT_LENGTH])));
	return OK;
}
