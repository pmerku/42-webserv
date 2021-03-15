//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPParser.hpp"

using namespace NotApache;

HTTPParser::ParseState HTTPParser::parse(HTTPClient &client) {
	(void)client;
	return READY_FOR_WRITE;
}
