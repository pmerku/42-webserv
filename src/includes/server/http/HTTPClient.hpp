//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "server/ServerTypes.hpp"
#include "utils/mutex.hpp"
#include "server/http/HTTPClientData.hpp"

namespace NotApache {

	enum ClientWriteState {
		IS_WRITING,
		NO_RESPONSE
	};
	enum ClientConnectionState {
		READING,
		WRITING,
		CLOSED
	};

	class HTTPClient {
	private:
		FD	_fd;

	public:
		ClientWriteState		writeState;
		ClientConnectionState	connectionState;
		utils::Mutex<bool>		isHandled;
		HTTPClientData			data;

		HTTPClient(FD clientFd);

		FD	getFd() const;
	};

}

#endif //HTTPCLIENT_HPP
