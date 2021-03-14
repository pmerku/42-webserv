//
// Created by jelle on 3/13/2021.
//

#include "HandlerHolder.hpp"

using namespace NotApacheRewrite;

HandlerHolder::HandlerHolder(): _roundRobin(0), _size(0), _handlers() {}

HandlerHolder::~HandlerHolder() {
	for (container::iterator i = _handlers.begin(); i != _handlers.end(); ++i)
		delete *i;
}

void HandlerHolder::addHandler(AHandler *handler) {
	_handlers.push_back(handler);
	_size = _handlers.size();
}

void HandlerHolder::handleClient(HTTPClient &client, handlerActions action) {
	_roundRobin = (_roundRobin+1 < _size) ? _roundRobin+1 : 0;
	AHandler	*handler = _handlers[_roundRobin];
	if (action == READ)
		handler->read(client);
	else if (action == WRITE)
		handler->write(client);
}
