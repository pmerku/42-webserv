//
// Created by jelle on 3/2/2021.
//

#ifndef AHANDLER_HPP
#define AHANDLER_HPP

#include "server/http/HTTPClient.hpp"
#include "server/http/HTTPParser.hpp"
#include "server/http/HTTPResponder.hpp"
#include "server/communication/ServerEventBus.hpp"

namespace NotApache {
	///	Handles the read and write of a client connection
	class AHandler {
	protected:
		HTTPParser		*_parser;
		HTTPResponder	*_responder;
		ServerEventBus	*_eventBus;

	public:
		AHandler();
		virtual ~AHandler();

		virtual void	read(HTTPClient &client) = 0;
		virtual void	write(HTTPClient &client) = 0;

		void setParser(HTTPParser *parser);
		void setResponder(HTTPResponder *responder);
		void setEventBus(ServerEventBus *eventBus);
	};
}

#endif //AHANDLER_HPP
