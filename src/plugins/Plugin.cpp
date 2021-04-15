//
// Created by pmerku on 14/04/2021.
//

#include "plugins/Plugin.hpp"

using namespace plugin;

Plugin::Plugin(const std::string &name) : _id(name) { }

Plugin::~Plugin() { }

const std::string &Plugin::getId() const {
	return _id;
}

bool Plugin::onHandleError(NotApache::HTTPClient &client, int code) {
	(void)client;
	(void)code;
	return false;
}

bool Plugin::onSendFile(NotApache::HTTPClient &client) {
	(void)client;
	return false;
}

bool Plugin::operator==(const std::string &name) const {
	return _id == name;
}

bool Plugin::operator==(const char *name) const {
	return operator==(std::string(name));
}
