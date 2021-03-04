//
// Created by jelle on 3/4/2021.
//

#include "server/responders/AResponder.hpp"

using namespace NotApache;

AResponder::AResponder(const std::string &type): _type(type) {}

AResponder::~AResponder() {}

const std::string &AResponder::getType() const {
	return _type;
}
