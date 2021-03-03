//
// Created by jelle on 3/2/2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "log/Loggable.hpp"
#include "server/FileDescriptor.hpp"
#include "server/AHandler.hpp"
#include "server/AListener.hpp"
#include "server/AParser.hpp"
#include "server/Client.hpp"
#include <vector>
#include <sys/socket.h>

namespace NotApache {

	///	Generic server, combines listeners, parsers and handlers
	class Server: public log::ILoggable {
	private:
		std::vector<AListener*>	_listeners;
		std::vector<AHandler*>	_handlers;
		std::vector<AParser*>	_parsers;

		std::vector<Client*>	_clients;
		fd_set					_readFDSet;
		fd_set					_writeFDSet;

		FD						_maxFD();
		void 					_createFDSets();

	public:
		Server();

		void	serve();

		void 	addListener(AListener *listener);
		void 	addHandler(AHandler *handler);
		void 	addParser(AParser *parser);
	};
}

#endif //SERVER_HPP
