//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include <config/blocks/ServerBlock.hpp>
#include "server/http/HTTPClient.hpp"

namespace NotApache {

	class HTTPResponder {
	public:
		static void	generateResponse(HTTPClient &client);

		static void generateAssociatedResponse(HTTPClient &client);

		static void serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &file);

		static void handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage = true);
	};

}

#endif //HTTPRESPONDER_HPP
