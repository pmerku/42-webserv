//
// Created by jelle on 3/12/2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "config/blocks/RootBlock.hpp"
#include "log/Logger.hpp"
#include "server/ServerTypes.hpp"
#include "server/communication/TCPListener.hpp"
#include "server/communication/ServerEventBus.hpp"
#include "server/handlers/HandlerHolder.hpp"
#include "server/http/HTTPParser.hpp"
#include "server/http/HTTPResponder.hpp"
#include "server/terminal/TerminalResponder.hpp"
#include "server/terminal/TerminalClient.hpp"
#include "server/global/GlobalLogger.hpp"
#include <sys/socket.h>
#include <vector>
#include <list>
#include <exception>

namespace NotApache {

	class Server {
	private:
		enum SelectReturn {
			SUCCESS,
			TIMEOUT,
			ERROR
		};

		// select fd sets
		fd_set	_readFdSet;
		fd_set	_writeFdSet;
		FD		_maxFd;
		bool 	_shouldShutdown;

		// parts
		std::vector<TCPListener *>	_listeners;
		std::list<HTTPClient *>		_clients;
		HandlerHolder				_handlers;
		ServerEventBus				_eventBus;

		// responders & parsers
		HTTPParser			_httpParser;
		HTTPResponder		_httpResponder;
		TerminalClient		_termClient;
		TerminalResponder	_termResponder;

		SelectReturn	_runSelect();
		void			_createFdSets();
		void			_handleSelect();
		void			_clientCleanup();

	public:
		Server();
		virtual ~Server();

		void 			addListener(TCPListener *listener);
		void 			addHandler(AHandler *handler);
		void 			setLogger(logger::Logger &logger);
		void			startServer(config::RootBlock *config);

		void			shutdownServer();

		class IoSelectingFailed: public std::exception {
		public:
			const char * what() const throw() {
				return "Failed to run select() on clients";
			}
		};
	};

}
#endif //SERVER_HPP
