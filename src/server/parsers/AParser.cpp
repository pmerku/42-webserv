//
// Created by jelle on 3/3/2021.
//

#include "server/parsers/AParser.hpp"
#include <vector>

using namespace NotApache;

AParser::AParser(ClientTypes type, const std::string &dataType): _type(type), _dataType(dataType) {}

ClientTypes AParser::getType() const {
	return _type;
}

void AParser::setType(ClientTypes type) {
	_type = type;
}

AParser::formatState	AParser::runFormatChecks(std::vector<AParser *> &parsers, Client &client) {
	int	parseErrors = 0;
	int parsersAvailable = 0;
	for (std::vector<AParser*>::iterator i = parsers.begin(); i < parsers.end(); ++i) {
		if ((*i)->getType() != client.getType()) continue;
		parsersAvailable++;
		switch ((*i)->formatCheck(client)) {
			case FINISHED:
				client.setDataType((*i)->getDataType());
				return FINISHED;
			case PARSE_ERROR:
				parseErrors++;
				break;
			default:
				break;
		}
	}
	if (parsersAvailable == 0 || parsersAvailable == parseErrors) {
		switch (client.getType()) {
			default:
			case CONNECTION:
				client.setDataType("HTTP");
				break;
			case TERMINAL:
				client.setDataType("COM");
				break;
		}
		return PARSE_ERROR;
	}
	return UNFINISHED;
}

std::string AParser::getDataType() const {
	return _dataType;
}

AParser::~AParser() {}
