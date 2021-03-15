//
// Created by jelle on 3/14/2021.
//

#ifndef HTTPCLIENTDATA_HPP
#define HTTPCLIENTDATA_HPP

#include <string>

namespace NotApache {

	class HTTPClientRequest {
	private:
		std::string				_request;

	public:
		HTTPClientRequest();

		const std::string		&getRequest() const;

		void					appendRequestData(const std::string	&newData);
		void					setRequest(const std::string &newData);
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

	class HTTPClientData {
	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;
	};

}



#endif //HTTPCLIENTDATA_HPP
