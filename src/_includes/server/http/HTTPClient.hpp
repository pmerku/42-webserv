//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "server/ServerTypes.hpp"
#include "utils/mutex.hpp"
#include "server/http/HTTPClientData.hpp"
#include "server/http/Proxy.hpp"
#include "server/http/CGIClass.hpp"
#include "netinet/in.h"
#include "config/blocks/ServerBlock.hpp"
#include "config/blocks/RouteBlock.hpp"
#include <vector>
#include <netinet/in.h>

namespace NotApache {

	enum ClientWriteState {
		IS_WRITING,
		NO_RESPONSE,
		GOT_ASSOCIATED
	};
	enum ClientConnectionState {
		READING,
		WRITING,
		ASSOCIATED_FD,
		CLOSED
	};

	enum ClientResponseState {
		PROXY,
		FILE,
		CGI,
		UPLOAD,
		NONE
	};

	struct associatedFD {
		FD	fd;
		typedef enum {
			READ,
			WRITE
		} type;
		type mode;
	};

	class HTTPClient {
	private:
		FD							_fd;
		int							_port;
		long 						_host;
		std::vector<associatedFD>	_associatedFds;
		time_t 						_createdAt;
		long 						_timeoutAfter;
		sockaddr_in					_cli_addr;

	public:
		ClientWriteState		writeState;
		ClientConnectionState	connectionState;
		ClientResponseState		responseState;
		utils::Mutex<bool>		isHandled;
		HTTPClientData			data;
		Proxy					*proxy;
		CgiClass				*cgi;
	    unsigned int            clientCount;
	    int                     concurrentFails;

	    // responder data
		config::ServerBlock		*serverBlock;
		config::RouteBlock		*routeBlock;
		int 					replyStatus;
		std::string				rewrittenUrl;
		utils::Uri				file;
		bool 					hasErrored;

		HTTPClient(FD clientFd, int port, long host, sockaddr_in cli_addr);
		~HTTPClient();

		FD	getFd() const;
		int getPort() const;
		long getHost() const;
		sockaddr_in getCliAddr() const;
		std::string getIp() const;
		void	setTimeout(int timeout);
		void 	endRequest(bool shouldClose);

		void	addAssociatedFd(FD fd, associatedFD::type mode = associatedFD::READ);
		void	removeAssociatedFd(FD fd);
		void	clearAssociatedFd();
		void 	setAssociatedFdMode(FD fd, associatedFD::type mode);
		associatedFD	getAssociatedFd(std::vector<associatedFD>::size_type i) const;
		std::vector<associatedFD>::size_type	getAssociatedFdLength() const;

		void 	    timeout(bool useLocks = true);
        long int    getTimeDiff() const;
        long int 	getTimeoutAfter() const;
	};

}

#endif //HTTPCLIENT_HPP
