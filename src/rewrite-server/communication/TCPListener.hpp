//
// Created by jelle on 3/12/2021.
//

#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include "rewrite-server/RewriteServerTypes.hpp"
#include "rewrite-server/http/HTTPClient.hpp"

namespace NotApacheRewrite {

	class TCPListener {
	private:
		int _port;
		FD _fd;
		int _backlog;
		sockaddr_in svr_addr;

	public:
		TCPListener(int port, int backLog = 5);
		virtual ~TCPListener();

		FD getFD();
		void start();

		HTTPClient	*acceptClient();

		class FailedToListen : std::exception {
			const char *what() const throw() {
				return "TCPListener: Failed to bind and listen on port";
			}
		};

		class FailedToAccept : std::exception {
			const char *what() const throw() {
				return "TCPListener: Failed to accept client";
			}
		};
	};

}

#endif //TCPLISTENER_HPP
