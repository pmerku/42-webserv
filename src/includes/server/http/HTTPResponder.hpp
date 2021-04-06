//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include <config/blocks/ServerBlock.hpp>
#include "server/http/HTTPClient.hpp"
#include <sys/stat.h>

namespace NotApache {

	class HTTPResponder {
	public:
		static void	generateResponse(HTTPClient &client);

		static void generateAssociatedResponse(HTTPClient &client);

		static void serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &file);

		static void handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage = true);
		static void handleError(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route, int code, bool doErrorPage = true);

		static void
		serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct ::stat &directoryStat, const std::string &dirPath);
		static void	prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const utils::Uri &file, int code = 200);
		static void	prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct ::stat &buf, const utils::Uri &file, int code = 200);
	};

}

#endif //HTTPRESPONDER_HPP
