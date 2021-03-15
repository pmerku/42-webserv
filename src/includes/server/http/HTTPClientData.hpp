//
// Created by jelle on 3/14/2021.
//

#ifndef HTTPCLIENTDATA_HPP
#define HTTPCLIENTDATA_HPP

#include <string>
#include <map>

namespace NotApache {
		enum e_method {
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE
	};
	enum e_headers {
		ACCEPT_CHARSET,
		ACCEPT_LANGUAGE,
		ALLOW,
		AUTHORIZATION,
		//CONNECTION,
		CONTENT_LANGUAGE,
		CONTENT_LENGTH,
		CONTENT_LOCATION,
		CONTENT_TYPE,
		DATE,
		HOST,
		LAST_MODIFIED,
		LOCATION,
		REFERER,
		RETRY_AFTER,		
		SERVER,
		TRANSFER_ENCODING,
		USER_AGENT,
		WWW_AUTHENTICATE
	};

	class HTTPClientRequest {
	private:
		std::string							_rawRequest;
		e_method							_method;
		std::string							_uri;
		std::pair<int, int>					_version;
		std::map<e_headers, std::string>	_headers;
		std::string							_body;
		size_t								_bodySize;
		std::map<std::string, e_method>		_methodMap;
		std::map<std::string, e_headers>	_headerMap;

	public:
		HTTPClientRequest();

		const std::string		&getRawRequest() const;

		void					appendRequestData(const std::string	&newData);
		void					setRawRequest(const std::string &newData);
	};

	class HTTPClientResponse {
	private:
		std::string				_response;
		std::string::size_type	_progress;

	public:
		HTTPClientResponse();

		const std::string		&getResponse() const;
		std::string::size_type	getProgress() const;

		void					setResponse(const std::string &response);
		void					setProgress(std::string::size_type index);
		int						parseRequest(std::string request);
		int						parseHeaders(std::string line);
		int						parseRequestLine(std::string reqLine);
		int						parseBody(std::string line);
	};

	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x);

	class HTTPClientData {
	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;
	};

}



#endif //HTTPCLIENTDATA_HPP
