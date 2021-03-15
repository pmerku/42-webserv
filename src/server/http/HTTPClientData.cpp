//
// Created by jelle on 3/14/2021.
//

#include "server/http/HTTPClientData.hpp"
#include <iostream>

namespace NotApache
{
	    std::string methodAsString(const e_method& in) {
        switch (in) {
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
            default:
                return "INVALID METHOD";
        }
	}

	std::string headerAsString(const e_headers& in) {
        switch (in) {
			case ACCEPT_CHARSET:
				return "ACCEPT-CHARSET";
			case ACCEPT_LANGUAGE:
				return "ACCEPT-LANGUAGE";
			case ALLOW:
				return "ALLOW";
			case AUTHORIZATION:
				return "AUTHORIZATION";
			//case //CONNECTION:
			//	return "CONNECTION";
			case CONTENT_LANGUAGE:
				return "CONTENT-LANGUAGE";
			case CONTENT_LENGTH:
				return "CONTENT-LENGTH";
			case CONTENT_LOCATION:
				return "CONTENT-LOCATION";
			case CONTENT_TYPE:
				return "CONTENT-TYPE";
			case DATE:
				return "DATE";
			case HOST:
				return "HOST";
			case LAST_MODIFIED:
				return "LAST-MODIFIED";
			case LOCATION:
				return "LOCATION";
			case REFERER:
				return "REFERER";
			case RETRY_AFTER:
				return "RETRY-AFTER";	
			case SERVER:
				return "SERVER";
			case TRANSFER_ENCODING:
				return "TRANSFER-ENCODING";
			case USER_AGENT:
				return "USER-AGENT";
			case WWW_AUTHENTICATE:
				return "WWW-AUTHENTICATE";
			default:
				return "INVALID HEADER";
        }
    }

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& x) {
		for (size_t i = 0; i < x.size(); ++i)
			o << x[i] << std::endl;
		return o;
	}

	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x) {
		o	<< "==REQUEST=="								<< std::endl
			<< "Method: "	<< methodAsString(x._method)	<< std::endl
			<< "URI: "		<< x._uri						<< std::endl
			<< "VERSION: HTTP/"	<< x._version.first << "." << x._version.second	<< std::endl;
			if (x._headers.size()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<e_headers, std::string>::iterator it = x._headers.begin(); it != x._headers.end(); ++it)
					std::cout << "Header: [" << headerAsString(it->first) << ": " << it->second << "]" << std::endl;
			}
			else
				std::cout << "-NO HEADERS-" << std::endl;
			if (x._bodySize)
				std::cout << "Body size: " << x._bodySize << std::endl;
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

HTTPClientRequest::HTTPClientRequest(): _rawRequest(), _method(), _uri(), _version(), _headers(), _body(), _bodySize(), _methodMap(), _headerMap(), _error_code(), _chunked(false) {
	_methodMap["GET"] = GET;
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
	_methodMap["DELETE"] = DELETE;
	_methodMap["CONNECT"] = CONNECT;
	_methodMap["OPTIONS"] = OPTIONS;
	_methodMap["TRACE"] = TRACE;
	_headerMap["ACCEPT-CHARSET"] = ACCEPT_CHARSET;
	_headerMap["ACCEPT-LANGUAGE"] = ACCEPT_LANGUAGE;
	_headerMap["ALLOW"] = ALLOW;
	_headerMap["AUTHORIZATION"] = AUTHORIZATION;
	//_headerMap["CONNECTION"] = CONNECTION;
	_headerMap["CONTENT-LANGUAGE"] = CONTENT_LANGUAGE;
	_headerMap["CONTENT-LENGTH"] = CONTENT_LENGTH;
	_headerMap["CONTENT-LOCATION"] = CONTENT_LOCATION;
	_headerMap["CONTENT-TYPE"] = CONTENT_TYPE;
	_headerMap["DATE"] = DATE;
	_headerMap["HOST"] = HOST;
	_headerMap["LAST-MODIFIED"] = LAST_MODIFIED;
	_headerMap["LOCATION"] = LOCATION;
	_headerMap["REFERER"] = REFERER;
	_headerMap["RETRY-AFTER"] = RETRY_AFTER;
	_headerMap["SERVER"] = SERVER;
	_headerMap["TRANSFER-ENCODING"] = TRANSFER_ENCODING;
	_headerMap["USER-AGENT"] = USER_AGENT;
	_headerMap["WWW-AUTHENTICATE"] = WWW_AUTHENTICATE;
}

const std::string &HTTPClientRequest::getRawRequest() const {
	return _rawRequest;
}

void HTTPClientRequest::appendRequestData(const std::string &newData) {
	_rawRequest += newData;
}

void HTTPClientRequest::setRawRequest(const std::string &newData) {
	_rawRequest = newData;
}

HTTPClientResponse::HTTPClientResponse(): _response(),  _progress(0) {}

const std::string &HTTPClientResponse::getResponse() const {
	return _response;
}

std::string::size_type HTTPClientResponse::getProgress() const {
	return _progress;
}

void HTTPClientResponse::setResponse(const std::string &response) {
	_response = response;
}

void HTTPClientResponse::setProgress(std::string::size_type index) {
	_progress = index;
}

