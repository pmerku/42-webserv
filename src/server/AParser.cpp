//
// Created by jelle on 3/3/2021.
//

#include "server/AParser.hpp"
#include <vector>

using namespace NotApache;

AParser::AParser(ClientTypes type): _type(type) {}

ClientTypes AParser::getType() const {
	return _type;
}

void AParser::setType(ClientTypes type) {
	_type = type;
}

AParser::parseState	AParser::runParsers(std::vector<AParser *> &parsers, Client &client) {
	int	parseErrors = 0;
	for (std::vector<AParser*>::iterator i = parsers.begin(); i < parsers.end(); ++i) {
		switch ((*i)->parse(client)) {
			case FINISHED:
				return FINISHED;
			case PARSE_ERROR:
				parseErrors++;
				break;
			default:
				break;
		}
	}

	if (parsers.size() == (unsigned long)parseErrors)
		return PARSE_ERROR;
	return UNFINISHED;
};
