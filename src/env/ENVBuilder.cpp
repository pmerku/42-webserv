//
// Created by pmerku on 22/03/2021.
//

#include "env/ENVBuilder.hpp"
#include "utils/strdup.hpp"
#include "utils/ErrorThrow.hpp"

#include <cstdlib>

using namespace CGIenv;

ENVBuilder::ENVBuilder() { }

ENVBuilder &ENVBuilder::AUTH_TYPE(const std::string &value) {
	_metaVariables.insert(std::make_pair("AUTH_TYPE", value));
	return *this;
}

ENVBuilder &ENVBuilder::CONTENT_LENGTH(const std::string &value) {
	_metaVariables.insert(std::make_pair("CONTENT_LENGTH", value));
	return *this;
}

ENVBuilder &ENVBuilder::CONTENT_TYPE(const std::string &value) {
	_metaVariables.insert(std::make_pair("CONTENT_TYPE", value));
	return *this;
}

ENVBuilder &ENVBuilder::GATEWAY_INTERFACE(const std::string &value) {
	_metaVariables.insert(std::make_pair("GATEWAY_INTERFACE", value));
	return *this;
}

ENVBuilder &ENVBuilder::PATH_INFO(const std::string &value) {
	_metaVariables.insert(std::make_pair("PATH_INFO", value));
	return *this;
}

ENVBuilder &ENVBuilder::PATH_TRANSLATED(const std::string &value) {
	_metaVariables.insert(std::make_pair("PATH_TRANSLATED", value));
	return *this;
}

ENVBuilder &ENVBuilder::QUERY_STRING(const std::string &value) {
	_metaVariables.insert(std::make_pair("QUERY_STRING", value));
	return *this;
}

ENVBuilder &ENVBuilder::REMOTE_ADDR(const std::string &value) {
	_metaVariables.insert(std::make_pair("REMOTE_ADDR", value));
	return *this;
}

ENVBuilder &ENVBuilder::REMOTE_INDENT(const std::string &value) {
	_metaVariables.insert(std::make_pair("REMOTE_INDENT", value));
	return *this;
}

ENVBuilder &ENVBuilder::REMOTE_USER(const std::string &value) {
	_metaVariables.insert(std::make_pair("REMOTE_USER", value));
	return *this;
}

ENVBuilder &ENVBuilder::REQUEST_METHOD(const std::string &value) {
	_metaVariables.insert(std::make_pair("REQUEST_METHOD", value));
	return *this;
}

ENVBuilder &ENVBuilder::REQUEST_URI(const std::string &value) {
	_metaVariables.insert(std::make_pair("REQUEST_URI", value));
	return *this;
}

ENVBuilder &ENVBuilder::SCRIPT_NAME(const std::string &value) {
	_metaVariables.insert(std::make_pair("SCRIPT_NAME", value));
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_NAME(const std::string &value) {
	_metaVariables.insert(std::make_pair("SERVER_NAME", value));
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_PORT(const std::string &value) {
	_metaVariables.insert(std::make_pair("SERVER_PORT", value));
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_PROTOCOL(const std::string &value) {
	_metaVariables.insert(std::make_pair("SERVER_PROTOCOL", value));
	return *this;
}

ENVBuilder &ENVBuilder::SERVER_SOFTWARE(const std::string &value) {
	_metaVariables.insert(std::make_pair("SERVER_SOFTWARE", value));
	return *this;
}

char **ENVBuilder::build() {
	char **envp;
	if (!(envp = static_cast<char **>(calloc(_metaVariables.size(), sizeof(char *)))))
		ERROR_THROW(MallocError());

	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _metaVariables.begin(); it != _metaVariables.end(); it++) {
		if (it->first.empty() || it->second.empty())
			ERROR_THROW(MallocError());

		if (!(envp[i] = static_cast<char *>(calloc(it->first.length() + 1 + it->second.length() + 1, sizeof(char))))) {
			for (; i >= 0; --i)
				free(envp[i]);
			free(envp);
			ERROR_THROW(MallocError());
		}

		std::string str = it->first + "=" + it->second;
		envp[i] = utils::strdup(str);

		i++;
	}

	return envp;
}
