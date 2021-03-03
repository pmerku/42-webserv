//
// Created by jelle on 3/3/2021.
//

#include "server/AHandler.hpp"

using namespace NotApache;

void	AHandler::setParsers(std::vector<AParser *> *parsers) {
	_parsers = parsers;
}

AHandler::AHandler(): _parsers(0) {}
