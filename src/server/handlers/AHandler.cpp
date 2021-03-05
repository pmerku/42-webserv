//
// Created by jelle on 3/3/2021.
//

#include "server/handlers/AHandler.hpp"

using namespace NotApache;

void	AHandler::setParsers(std::vector<AParser *> *parsers) {
	_parsers = parsers;
}

void AHandler::setResponders(std::vector<AResponder *> *responders) {
	_responders = responders;
}

AHandler::AHandler(): _parsers(0), _responders(0) {}

void AHandler::respond(Client &client) {
	if (!_responders) return;
	for (std::vector<AResponder*>::iterator i = _responders->begin(); i != _responders->end(); ++i) {
		if ((*i)->getType() == client.getDataType()) {
			if (client.getResponseState() == PARSE_ERROR)
				client.setResponse((*i)->generateParseError(client));
			else
				client.setResponse((*i)->generateResponse(client));
			client.setResponseIndex(0);
			client.setResponseState(IS_WRITING);
			return;
		}
	}
	client.setResponseState(ERRORED);
}

AHandler::~AHandler() {}
