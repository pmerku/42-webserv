//
// Created by jelle on 3/3/2021.
//

#include "rewrite-server/handlers/AHandler.hpp"

using namespace NotApacheRewrite;

AHandler::AHandler(): _parser(0), _responder(0), _eventBus(0) {}
AHandler::~AHandler() {}

void AHandler::setParser(HTTPParser *parser) {
	_parser = parser;
}

void AHandler::setResponder(HTTPResponder *responder) {
	_responder = responder;
}

void AHandler::setEventBus(ServerEventBus *eventBus) {
	_eventBus = eventBus;
}
