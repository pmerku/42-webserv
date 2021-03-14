//
// Created by jelle on 3/12/2021.
//

#ifndef REWRITESERVER_HPP
#define REWRITESERVER_HPP

#include "config/blocks/RootBlock.hpp"
#include "rewrite-server/RewriteServerTypes.hpp"
#include "rewrite-server/communication/TCPListener.hpp"
#include "rewrite-server/communication/ServerEventBus.hpp"
#include "rewrite-server/handlers/HandlerHolder.hpp"
#include "rewrite-server/http/HTTPParser.hpp"
#include "rewrite-server/http/HTTPResponder.hpp"
#include "rewrite-server/terminal/TerminalResponder.hpp"
#include "rewrite-server/terminal/TerminalClient.hpp"
#include "rewrite-server/global/GlobalConfig.hpp"
#include <sys/socket.h>
#include <vector>
#include <list>
#include <exception>

namespace NotApacheRewrite {

	class RewriteServer {
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
		RewriteServer();
		virtual ~RewriteServer();

		void 			addListener(TCPListener *listener);
		void 			addHandler(AHandler *handler);
		void			startServer(config::RootBlock *config);

		class IoSelectingFailed: public std::exception {
		public:
			const char * what() const throw() {
				return "Failed to run select() on clients";
			}
		};
	};

}
#endif //REWRITESERVER_HPP
