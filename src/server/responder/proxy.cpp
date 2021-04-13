//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "utils/intToString.hpp"

using namespace NotApache;

void HTTPResponder::handleProxy(HTTPClient &client) {
	globalLogger.logItem(logger::DEBUG, "Handling the proxy connection");

	try {

		client.proxy = new Proxy(client.routeBlock->getProxyUrl().ip, client.routeBlock->getProxyUrl().port);
		client.proxy->createConnection();

		client.addAssociatedFd(client.proxy->getSocket(), associatedFD::WRITE);
		client.responseState = PROXY;
		client.connectionState = ASSOCIATED_FD;

		std::string host = client.routeBlock->getProxyUrl().ip + ":" + utils::intToString(client.routeBlock->getProxyUrl().port);
		std::string x_client = client.getIp();
		std::string x_host = client.data.request.data.headers.find("HOST")->second;
		std::string x_proto = client.routeBlock->getProxyUrl().protocol;

		client.proxy->request.setRequest(
		RequestBuilder(client.data.request.data)
				.setHeader("HOST", host)
				.setHeader("CONNECTION", "Close") // always set so it doesn't hang
				.setHeader("X-FORWARDED-FOR", x_client)
				.setHeader("X-FORWARDED-HOST", x_host)
				.setHeader("X-FORWARDED-PROTO", x_proto)
				.build()
		);

	} catch (Proxy::SocketException &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, 500);
	} catch (Proxy::ConnectionException &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, 502);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, 500);
	}
}
