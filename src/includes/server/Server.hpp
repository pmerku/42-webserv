//
// Created by jelle on 3/2/2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "log/Loggable.hpp"
#include "server/ServerTypes.hpp"
#include "server/handlers/AHandler.hpp"
#include "server/listeners/AListener.hpp"
#include "server/parsers/AParser.hpp"
#include "server/responders/AResponder.hpp"
#include "server/Client.hpp"
#include <vector>
#include <sys/socket.h>

namespace NotApache {

	///	Generic server, combines listeners, parsers and handlers
	class Server: public log::ILoggable {
	private:
		std::vector<AListener*>		_listeners;
		std::vector<AHandler*>		_handlers;
		std::vector<AParser*>		_parsers;
		std::vector<AResponder*>	_responders;
		std::vector<AHandler*>::size_type	_handlerBalance;

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
		void 	addResponder(AResponder *responder);
	};
}

#endif //SERVER_HPP
