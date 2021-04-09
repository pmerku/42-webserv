//
// Created by pmerku on 22/03/2021.
//

#include "env/ENVBuilder.hpp"
#include "utils/strdup.hpp"

using namespace CGIenv;

ENVBuilder::ENVBuilder() { }

ENVBuilder &ENVBuilder::AUTH_TYPE(const std::string &value) {
	_metaVariables["AUTH_TYPE"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::CONTENT_LENGTH(const std::string &value) {
	_metaVariables["CONTENT_LENGTH"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::CONTENT_TYPE(const std::string &value) {
	_metaVariables["CONTENT_TYPE"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::GATEWAY_INTERFACE(const std::string &value) {
	_metaVariables["GATEWAY_INTERFACE"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::PATH_INFO(const std::string &value) {
	_metaVariables["PATH_INFO"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::PATH_TRANSLATED(const std::string &value) {
	_metaVariables["PATH_TRANSLATED"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::QUERY_STRING(const std::string &value) {
	_metaVariables["QUERY_STRING"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::REMOTE_ADDR(const std::string &value) {
	_metaVariables["REMOTE_ADDR"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::REMOTE_IDENT(const std::string &value) {
	_metaVariables["REMOTE_IDENT"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::REMOTE_USER(const std::string &value) {
	_metaVariables["REMOTE_USER"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::REQUEST_METHOD(const std::string &value) {
	_metaVariables["REQUEST_METHOD"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::REQUEST_URI(const std::string &value) {
	_metaVariables["REQUEST_URI"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::SCRIPT_NAME(const std::string &value) {
	_metaVariables["SCRIPT_NAME"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_NAME(const std::string &value) {
	_metaVariables["SERVER_NAME"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_PORT(const std::string &value) {
	_metaVariables["SERVER_PORT"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_PROTOCOL(const std::string &value) {
	_metaVariables["SERVER_PROTOCOL"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_SOFTWARE(const std::string &value) {
	_metaVariables["SERVER_SOFTWARE"] = value;
	return *this;
}

ENVBuilder &ENVBuilder::REDIRECT_STATUS(const std::string& value) {
	_metaVariables["REDIRECT_STATUS"] = value;
	return *this;
}

char **ENVBuilder::build() {
	char **envp = new char *[_metaVariables.size() + 1]();

	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _metaVariables.begin(); it != _metaVariables.end(); it++) {
		if (it->first.empty() || it->second.empty())
			continue;

		std::string str = it->first + "=" + it->second;
		envp[i] = utils::strdup(str);

		i++;
	}

	return envp;
}
