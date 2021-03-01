//
// Created by jelle on 3/1/2021.
//

#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "Loggable.hpp"

class TCPServer: public log::ILoggable {
public:
	TCPServer();

	void	listen(int port);
};

#endif //TCPSERVER_HPP
