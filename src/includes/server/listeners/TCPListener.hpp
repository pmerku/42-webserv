//
// Created by jelle on 3/2/2021.
//

#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

#include <netinet/in.h>
#include <exception>
#include "server/listeners/AListener.hpp"
#include "log/Loggable.hpp"

namespace NotApache {

	///	Listener specialized for TCP sockets
	class TCPListener: public AListener {
	private:
		int 		_port;
		FD			_fd;
		int 		_backlog;
		sockaddr_in	svr_addr;

	public:
		TCPListener(int port, int backLog = 5);
		~TCPListener();

		FD		getFD();
		void	start();
		Client	*acceptClient();

		class FailedToListen: std::exception {
			const char * what() const throw() {
				return "TCPListener: Failed to bind and listen on port";
			}
		};

		class FailedToAccept: std::exception {
			const char * what() const throw() {
				return "TCPListener: Failed to accept client";
			}
		};
	};
}
#endif //TCPLISTENER_HPP
