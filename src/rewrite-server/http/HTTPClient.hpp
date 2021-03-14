//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "rewrite-server/RewriteServerTypes.hpp"
#include "utils/mutex.hpp"

namespace NotApacheRewrite {

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
		utils::MutexLock		modifyLock;

		HTTPClient(FD clientFd);

		FD	getFd() const;
	};

}

#endif //HTTPCLIENT_HPP
