//
// Created by martyparty on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"

namespace NotApache
{
	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x) {
		o	<< "==REQUEST=="													<< std::endl
			<< "Method: "	<< HTTPParser::methodMap_EtoS.find(x._method)->second 	<< std::endl
			<< "URI: "		<< x._uri											<< std::endl;

			if (x._headers.size()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<std::string, std::string>::iterator it = x._headers.begin(); it != x._headers.end(); ++it)
					o << "Header: [" << it->first << ": " << it->second << "]" 	<< std::endl;
			}
			else
				o	<< "-NO HEADERS-" 											<< std::endl;
			if (x._body.length()) {
				o	<< "Body length: " << x._body.length() 						<< std::endl << std::endl
					<< "-BODY-" 												<< std::endl 
					<< x._body 													<< std::endl;
			}
			else
				o << std::endl << "-NO BODY-" 									<< std::endl;
		return o;
	}
}

using namespace NotApache;

const std::string HTTPParser::allowedURIChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~!#$&'()*+,/:;=?@[]";

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
	// TODO check later
	if (client.data.request._rawRequest.length() > MAX_REQUEST)	{
		globalLogger.logItem(logger::DEBUG, "Content-length exceeds max body size");
		client.data.request._statusCode = 413;
		return ERROR;
	}
	if (client.data.request._isChunked)
		return (parseChunkedBody(client.data.request, client.data.request._rawRequest));
	else
		return (parseRequest(client.data.request));
}

HTTPParser::ParseState		HTTPParser::parseChunkedBody(HTTPClientRequest& req, std::string rawRequest) {
	size_t SOB = 0; // Start Of Body
	size_t EOB = 0; // End Of Body

	// Check for terminating character
	SOB = rawRequest.find_first_of("\r\n");
	if (SOB == std::string::npos)
	{
		globalLogger.logItem(logger::DEBUG, "No terminating character in body");
		req._statusCode = 400;
		return ERROR;
	}

	// Get chunksize
	std::string size = rawRequest.substr(0, SOB);
	if (size[0] != '0' && size[1] != 'x')
	{
		globalLogger.logItem(logger::DEBUG, "Failed to parse chunksize");
		req._statusCode = 400;
		return ERROR;
	}
	size_t chunkSize = utils::stoh(size.substr(2));
	if (req._body.length() + chunkSize > MAX_BODY) {
		globalLogger.logItem(logger::DEBUG, "Content-length exceeds max body size");
		req._statusCode = 413;
		return ERROR;
	}
	SOB+=2;

	EOB = rawRequest.rfind("\r\n0\r\n\r\n");
	// Check if chunksize matches body
	if (rawRequest.rfind("\r\n")-SOB != chunkSize && EOB-SOB != chunkSize) {
		globalLogger.logItem(logger::DEBUG, "Chunk size invalid");
		req._statusCode = 400;
		return ERROR;
	}

	// Append body
	req._body += rawRequest.substr(SOB, chunkSize);

	// Check if body is complete
	if (EOB != std::string::npos) // 0x0?
	{
		globalLogger.logItem(logger::DEBUG, "Succesfully parsed chunked body");
		return READY_FOR_WRITE;
	}
	else
	{
		globalLogger.logItem(logger::DEBUG, "UNFINISHED");
		req._rawRequest.clear();
		return UNFINISHED;
	}
}

HTTPParser::ParseState		HTTPParser::parseBody(HTTPClientRequest& req, std::string rawRequest) {
	size_t contentLength = utils::stoi(req._headers["CONTENT-LENGTH"]);
	if (rawRequest.length() < contentLength)
		return UNFINISHED;
	if (contentLength > MAX_BODY) {
		req._statusCode = 413;
		globalLogger.logItem(logger::ERROR, "Content-length exceeds max body size");
		return ERROR;
	}
	req._body += rawRequest.substr(0, contentLength);
	return READY_FOR_WRITE;
}

HTTPParser::ParseState		HTTPParser::parseHeaders(HTTPClientRequest& req, std::string rawRequest) {
	if (rawRequest.length() > MAX_HEADER) {
		req._statusCode = 431;
		globalLogger.logItem(logger::ERROR, "Headers exceed max header size");
		return ERROR;
	}
	
	std::vector<std::string> headers = utils::split(rawRequest, "\r\n");
	
	for (size_t i = 0; i < headers.size(); ++i) {
		//Check for colon
		if (headers[i].find(":") == std::string::npos) {
			req._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "no \":\" in header line");
			return ERROR;
		}
		//Check for spaces in field-name
		if (utils::countSpaces(headers[i].substr(0, headers[i].find_first_of(":")))) {
			req._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "Spaces in field-name" );
			return ERROR;
		}

		std::string fieldName = headers[i].substr(0, headers[i].find_first_of(":"));
		for (size_t i = 0; fieldName[i]; ++i)
			fieldName[i] = utils::toUpper(fieldName[i]);

		req._headers[fieldName] = headers[i].substr(headers[i].find_first_not_of(" ", fieldName.length() + 1), headers[i].find_last_not_of(" "));
		// ^ Set header ^
	}

	std::map<std::string, std::string>::iterator it = req._headers.find("TRANSFER-ENCODING");
	if (it != req._headers.end() && it->second.find("chunked") != std::string::npos)
		req._isChunked = true;

	it = req._headers.find("CONTENT-LENGTH");
	if (it != req._headers.end()) {
		if (req._isChunked) {
			req._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "Headers Transfer-encoding + Content-length not allowed");
			return ERROR;
		}
	}
	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequestLine(HTTPClientRequest& req, std::string rawRequest) {
	// CHECKING GLOBAL FORMAT
	size_t spaces = utils::countSpaces(rawRequest);
	std::vector<std::string> parts = utils::split(rawRequest, " ");
	if (spaces != 2 || parts.size() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid request line");
		req._statusCode = 400;
		return ERROR;
	}

	// Check Method
	if (methodMap_StoE.find(parts[0]) == methodMap_StoE.end()) {
		req._statusCode = 501; // 501 (Not Implemented)
		globalLogger.logItem(logger::ERROR, "Invalid method");
		return ERROR;
	}
	// Set Method
	req._method = methodMap_StoE.find(parts[0])->second;

	// CHECK URI
	if (parts[1][0] != '/') {
		req._statusCode = 400;
		globalLogger.logItem(logger::ERROR, "Invalid URI");
		return ERROR;
	}

	for (size_t i = 0; i < parts[1].size(); ++i) {
		if (allowedURIChars.find(parts[1][i]) == std::string::npos) {
			req._statusCode = 400;
			globalLogger.logItem(logger::ERROR, "Invalid char in URI");
			return ERROR;
		}
	}

	// Set URI
	req._uri = parts[1];

	// CHECK PROTOCOL
	if (parts[2] != "HTTP/1.1") {
		globalLogger.logItem(logger::ERROR, "Invalid protocol");
		req._statusCode = 505;
		return ERROR;
	}
	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequest(HTTPClientRequest& req) {
	std::string request = req._rawRequest;
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield

	EOH = request.find("\r\n\r\n");
	if (EOH == std::string::npos) {
		globalLogger.logItem(logger::DEBUG, "UNFINISHED: no EOF (\"\\r\\n\\r\\n\") in request");
		return UNFINISHED;
	}
	EOR = request.find("\r\n");
	if (parseRequestLine(req, request.substr(0, EOR)) == ERROR)
		return ERROR;

	if (EOR == EOH) {
		globalLogger.logItem(logger::DEBUG, "Request line only");
		return READY_FOR_WRITE;
	}
	else if (parseHeaders(req, request.substr(EOR, EOH - EOR)) == ERROR)
		return ERROR;

	if (req._isChunked)
		return (parseChunkedBody(req, request.substr(EOH + 4)));
	else if (req._headers.find("CONTENT-LENGTH") != req._headers.end())
		return (parseBody(req, request.substr(EOH + 4)));
	else
		return READY_FOR_WRITE;
}
