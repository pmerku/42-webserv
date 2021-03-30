//
// Created by jelle on 3/14/2021.
//

#ifndef HTTPCLIENTDATA_HPP
#define HTTPCLIENTDATA_HPP

#include "server/ServerTypes.hpp"
#include "utils/DataList.hpp"
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
	public:
		std::string							_rawRequest;
		e_method							_method;
		std::string							_uri;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_statusCode;
		bool								_isChunked;

		HTTPClientRequest();

		friend class HTTPParser;

		const std::string		&getRawRequest() const;

		void					appendRequestData(const std::string	&newData);
		void					setRawRequest(const std::string &newData);

		friend std::ostream& 	operator<<(std::ostream& o, HTTPClientRequest& x);
	};

	class HTTPClientResponse {
		enum responseType {
			FILE,
			CGI_FILE,
			PROXY
		};

	private:
		utils::DataList 			_response;
		utils::DataList 			_associatedData;

	public:
		HTTPClientResponse();

		bool 						hasProgress;
		utils::DataList::size_type	packetProgress;
		utils::DataList::iterator 	currentPacket;
		
		utils::DataList				&getResponse();

		void						setResponse(const utils::DataList &response);
		utils::DataList				&getAssociatedDataRaw();
		void						appendAssociatedData(const char *data, utils::DataList::size_type size);
		
		FD							_fd;
		FD							_bodyfd;
	};

	std::ostream& operator<<(std::ostream& o, HTTPClientRequest& x);

	class HTTPClientData {
	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;
	};

}



#endif //HTTPCLIENTDATA_HPP
