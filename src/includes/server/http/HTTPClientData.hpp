//
// Created by jelle on 3/14/2021.
//

#ifndef HTTPCLIENTDATA_HPP
#define HTTPCLIENTDATA_HPP

#include <string>
#include <map>

namespace NotApache {
		enum e_method {
		INVALID,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE
	};

	class HTTPClientRequest {
	private:
		std::string							_rawRequest;
		e_method							_method;
		std::string							_uri;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_statusCode;
		bool								_isChunked;

	public:
		HTTPClientRequest();
		friend class HTTPParser;
		const std::string		&getRawRequest() const;

		void					appendRequestData(const std::string	&newData);
		void					setRawRequest(const std::string &newData);

		friend std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x);
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
	};

	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x);

	class HTTPClientData {
	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;
	};

}



#endif //HTTPCLIENTDATA_HPP
