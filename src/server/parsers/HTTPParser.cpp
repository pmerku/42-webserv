//
// Created by jelle on 3/3/2021.
//

#include "server/parsers/HTTPParser.hpp"

using namespace NotApache;

HTTPParser::HTTPParser(): AParser(CONNECTION, "HTTP") {}

AParser::formatState HTTPParser::formatCheck(Client &client) const {
	const std::string	expected = "GET / HTTP/1.1\n";
	const std::string	&request = client.getRequest();

	for (std::string::size_type i = 0; i < request.length(); ++i) {
		if (i >= expected.length())
			return FINISHED;
		if (request[i] != expected[i])
			return PARSE_ERROR;
	}

	if (expected.length() == request.length())
		return FINISHED;
	return UNFINISHED;
}
