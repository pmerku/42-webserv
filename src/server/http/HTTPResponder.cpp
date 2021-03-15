//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"

using namespace NotApache;

void HTTPResponder::generateResponse(HTTPClient &client) {
	client.data.response.setResponse("Hello world\n");
}
