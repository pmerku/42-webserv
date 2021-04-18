//
// Created by jelle on 3/14/2021.
//

#ifndef HTTPCLIENTDATA_HPP
#define HTTPCLIENTDATA_HPP

#include "server/ServerTypes.hpp"
#include "utils/DataList.hpp"
#include "server/http/RequestBuilder.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/http/HTTPParseData.hpp"
#include <string>
#include <map>

namespace NotApache {

	class HTTPClientRequest {
	private:
		utils::DataList	_associatedData;

	public:
		HTTPParseData				data;
		bool 						hasProgress;
		utils::DataList::size_type	packetProgress;
		utils::DataList::iterator 	currentPacket;
		RequestBuilder				builder;

		HTTPClientRequest();

		friend class HTTPParser;

		utils::DataList			&getRequest();

		void					setRequest(const utils::DataList &request);
		utils::DataList			&getAssociatedDataRaw();
		void					appendAssociatedData(const char *data, utils::DataList::size_type size);
		void 					appendRequestData(const char *data, utils::DataList::size_type size);
	};

	class HTTPClientResponse {
		enum responseType {
			FILE,
			CGI_FILE,
			PROXY
		};

	private:
		utils::DataList 		_associatedData;

	public:
		HTTPClientResponse();
		explicit HTTPClientResponse(HTTPParseData::HTTPParseType type);

		HTTPParseData				data;
		bool 						hasProgress;
		utils::DataList::size_type	packetProgress;
		utils::DataList::iterator 	currentPacket;
		
		FD							_fd;
		FD							_bodyfd;
		ResponseBuilder				builder;

		utils::DataList			&getResponse();

		void					setResponse(const utils::DataList &response);
		utils::DataList			&getAssociatedDataRaw();
		void					appendAssociatedData(const char *data, utils::DataList::size_type size);
		void 					appendResponseData(const char *data, utils::DataList::size_type size);
	};

	class HTTPClientData {
	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;

		void reset();
	};

}



#endif //HTTPCLIENTDATA_HPP
