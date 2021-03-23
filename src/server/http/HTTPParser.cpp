//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"
#include "server/global/GlobalLogger.hpp"

#include <sstream>

using namespace NotApache;

HTTPParser::ParseState		 HTTPParser::parse(HTTPClient& client) {
	//_R = &client.data.request;
	
	if (client.data.request._isChunked)
		return (parseChunkedBody(client.data.request, client.data.request._rawRequest));
	else
		return (parseRequest(client.data.request));
}

HTTPParser::ParseState		HTTPParser::parseChunkedBody(HTTPClientRequest& _R, std::string rawRequest) {
	size_t SOB = 0; // Start Of Body

	// Check for terminating character
	SOB = rawRequest.find_first_of("\r\n");
	if (SOB == std::string::npos)
	{
		globalLogger.logItem(logger::INFO, "No terminating character in body");
		_R._statusCode = 400; // 400 (Bad Request)
		return ERROR;
	}

	// Get chunksize
	std::string size = rawRequest.substr(0, SOB);
	if (size[0] != '0' && size[1] != 'x')
	{
		globalLogger.logItem(logger::INFO, "Failed to parse chunksize");
		_R._statusCode = 400; // 400 (Bad Request)
		return ERROR;
	}
	size_t chunkSize = ft::stoh(size.substr(2));
	SOB+=2;

	// Check if chunksize matches body
	if (rawRequest.rfind("\r\n")-SOB != chunkSize && rawRequest.rfind("\r\n0\r\n\r\n")-SOB != chunkSize)
	{
		globalLogger.logItem(logger::INFO, "Body invalid");
		_R._statusCode = 400; // 400 (Bad Request)
		return ERROR;
	}

	// Append body
	_R._body += rawRequest.substr(SOB, chunkSize);
	_R._bodySize += chunkSize;

	// Check if body is complete
	if (rawRequest.rfind("\r\n0\r\n\r\n") != std::string::npos) // 0x0?
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
	size_t EOB = 0; // End Of Body

	_R._bodySize = ft::stoi(_R._headers["CONTENT-LENGTH"]);
	EOB = rawRequest.rfind("\r\n");
	if (EOB == std::string::npos)
		return ERROR;
	std::string body = rawRequest.substr(0, EOB);

	if (body.length() != _R._bodySize) {
		globalLogger.logItem(logger::ERROR, "Body size doesn't match");
		return ERROR;
	}
	_R._body += body;
	return READY_FOR_WRITE;
}

HTTPParser::ParseState		HTTPParser::parseHeaders(HTTPClientRequest& _R, std::string rawRequest) {
	std::vector<std::string> headers = ft::split(rawRequest, "\r\n");
	
	for (size_t i = 0; i < headers.size(); ++i) {
		//Check for colon
		if (headers[i].find(":") == std::string::npos) {
			_R._statusCode = 400; // 400 (Bad Request)
			globalLogger.logItem(logger::ERROR, "no \":\" in header line");
			return ERROR;
		}
		//Check for spaces in field-name
		if (ft::countWS(headers[i].substr(0, headers[i].find_first_of(":")))) {
			_R._statusCode = 400; // 400 (Bad Request)
			globalLogger.logItem(logger::ERROR, "Spaces in field-name" );
			return ERROR;
		}

		std::string fieldName = headers[i].substr(0, headers[i].find_first_of(":"));
		for (size_t i = 0; fieldName[i]; ++i)
			fieldName[i] = std::toupper(fieldName[i]); // std::toupper vervangen

		_R._headers[fieldName] = headers[i].substr(headers[i].find_first_not_of(" ", fieldName.length()+1), headers[i].find_last_not_of(" "));
		// ^ Set header ^
	}

	std::map<std::string, std::string>::iterator it = _R._headers.find("TRANSFER-ENCODING");
	if (it != _R._headers.end() && it->second.find("chunked") != std::string::npos)
		_R._isChunked = true;

	it = _R._headers.find("CONTENT-LENGTH");
	if (it != _R._headers.end()) {
		if (_R._isChunked) {
			_R._statusCode = 400; // 400 (Bad Request)
			globalLogger.logItem(logger::ERROR, "Headers Transfer-encoding + Content-length not allowed");
			return ERROR;
		}
		_R._bodySize = ft::stoi(it->second);
	}
	return OK;
}

HTTPParser::ParseState		HTTPParser::parseRequestLine(HTTPClientRequest& _R, std::string rawRequest) {
	// CHECKING GLOBAL FORMAT
	size_t spaces = ft::countWS(rawRequest);
	std::vector<std::string> parts = ft::split(rawRequest, " ");
	if (spaces != 2 || parts.size() != 3) {
		globalLogger.logItem(logger::ERROR, "Invalid request line");
		_R._statusCode = 400; // 400 (Bad Request) error or a 301 (Moved Permanently)
		return ERROR;
	}

	// Check Method
	if (_R._methodMap.find(parts[0]) == _R._methodMap.end()) {
		_R._statusCode = 501; // 501 (Not Implemented)
		globalLogger.logItem(logger::ERROR, "Invalid method");
		return ERROR;
	}
	// Set Method
	_R._method = _R._methodMap.find(parts[0])->second;

	// CHECK URI
	//	stat()?, "*", URI + HOST? URI > max_len = 414 (URI Too Long) status code

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
		globalLogger.logItem(logger::INFO, "No EOF (\"\\r\\n\\r\\n\") in request");
		return UNFINISHED;
	}
	EOR = request.find("\r\n");
	if (parseRequestLine(_R, request.substr(0, EOR)) == ERROR)
		return ERROR;
	EOR += 2;

	if (request.find("\r\n\r\n", EOR) == std::string::npos) {
		globalLogger.logItem(logger::INFO, "Request line only");
		return READY_FOR_WRITE;
	}
	else if (parseHeaders(_R, request.substr(EOR, EOH-EOR)) == ERROR) 
		return ERROR;
	EOH += 4;

	if (_R._isChunked)
		return (parseChunkedBody(_R, request.substr(EOH)));
	else if (_R._headers.find("CONTENT-LENGTH") != _R._headers.end())
		return (parseBody(_R, request.substr(EOH)));
	else
		return READY_FOR_WRITE;
}


/*
	Historically, HTTP header field values could be extended over
	multiple lines by preceding each extra line with at least one space
	or horizontal tab (obs-fold).  This specification deprecates such
	line folding except within the message/http media type
	(Section 8.3.1).  A sender MUST NOT generate a message that includes
	line folding (i.e., that has any field-value that contains a match to
	the obs-fold rule) unless the message is intended for packaging
	within the message/http media type.

	A server that receives an obs-fold in a request message that is not
	within a message/http container MUST either reject the message by
	sending a 400 (Bad Request), preferably with a representation
	explaining that obsolete line folding is unacceptable, or replace
	each received obs-fold with one or more SP octets prior to
	interpreting the field value or forwarding the message downstream.






	3.2.6.  Field Value Components

   Most HTTP header field values are defined using common syntax
   components (token, quoted-string, and comment) separated by
   whitespace or specific delimiting characters.  Delimiters are chosen
   from the set of US-ASCII visual characters not allowed in a token
   (DQUOTE and "(),/:;<=>?@[\]{}").

     token          = 1*tchar

     tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
                    / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
                    / DIGIT / ALPHA
                    ; any VCHAR, except delimiters

   A string of text is parsed as a single value if it is quoted using
   double-quote marks.

     quoted-string  = DQUOTE *( qdtext / quoted-pair ) DQUOTE
     qdtext         = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
     obs-text       = %x80-FF

   Comments can be included in some HTTP header fields by surrounding
   the comment text with parentheses.  Comments are only allowed in
   fields containing "comment" as part of their field value definition.

     comment        = "(" *( ctext / quoted-pair / comment ) ")"
     ctext          = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text

   The backslash octet ("\") can be used as a single-octet quoting
   mechanism within quoted-string and comment constructs.  Recipients
   that process the value of a quoted-string MUST handle a quoted-pair
   as if it were replaced by the octet following the backslash.

     quoted-pair    = "\" ( HTAB / SP / VCHAR / obs-text )

   A sender SHOULD NOT generate a quoted-pair in a quoted-string except
   where necessary to quote DQUOTE and backslash octets occurring within
   that string.  A sender SHOULD NOT generate a quoted-pair in a comment
   except where necessary to quote parentheses ["(" and ")"] and
   backslash octets occurring within that comment.
*/
