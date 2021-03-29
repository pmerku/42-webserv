//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/ServerBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/Unique.hpp"
#include "config/validators/RequiredKey.hpp"
#include "config/validators/IntValidator.hpp"
#include "config/validators/IsFile.hpp"
#include "config/validators/IpValidator.hpp"
#include "utils/stoi.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	ServerBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("host", ConfigValidatorListBuilder()
			  .add(new ArgumentLength(1))
			  .add(new Unique())
			  .add(new IpValidator(0))
			  .build())
	  	.addKey("port", ConfigValidatorListBuilder()
			  .add(new ArgumentLength(1))
			  .add(new Unique())
			  .add(new IntValidator(0, 1, 65535))
			  .build())
		.addKey("server_name", ConfigValidatorListBuilder() // TODO default to something & [a-Z0-9\-\.]+ validator
			.add(new ArgumentLength(1))
			.add(new Unique())
			.build())
		.addKey("error_page", ConfigValidatorListBuilder() // TODO status code validator
			.add(new ArgumentLength(2))
			.add(new IsFile(1))
			.add(new IntValidator(0, 400, 600))
			.build())
		.addKey("body_limit", ConfigValidatorListBuilder()
				.add(new ArgumentLength(1))
				.add(new IntValidator(0, 0, 600))
				.build())
	  	.build();

const AConfigBlock::validatorListType 	ServerBlock::_blockValidators =
		ConfigValidatorListBuilder()
		.add(new RequiredKey("host"))
		.add(new RequiredKey("port"))
		.build();

const std::string 						ServerBlock::_allowedBlocks[] = { "route", "" };

const AConfigBlock::validatorsMapType	&ServerBlock::getValidators() const {
	return ServerBlock::_validators;
}

const std::string						*ServerBlock::getAllowedBlocks() const {
	return ServerBlock::_allowedBlocks;
}

ServerBlock::ServerBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent) {}

std::string ServerBlock::getType() const {
	return "server";
}

const AConfigBlock::validatorListType &ServerBlock::getBlockValidators() const {
	return _blockValidators;
}

void	ServerBlock::cleanup() {
	for (validatorsMapType::const_iterator i = _validators.begin(); i != _validators.end(); ++i) {
		for (validatorListType::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			delete *j;
		}
	}
	for (validatorListType::const_iterator i = _blockValidators.begin(); i != _blockValidators.end(); ++i) {
		delete *i;
	}
}

// TODO host ip parsing
void ServerBlock::parseData() {
	_serverName = "_";
	_bodyLimit = -1;
	_errorPages.clear();

	_port = utils::stoi(getKey("port")->getArg(0));
	_host = getKey("host")->getArg(0);

	if (hasKey("server_name"))
		_serverName = getKey("server_name")->getArg(0);
	if (hasKey("body_limit"))
		_bodyLimit = utils::stoi(getKey("body_limit")->getArg(0));

	for (std::vector<ConfigLine>::const_iterator i = _lines.begin(); i != _lines.end(); ++i) {
		if (i->getKey() != "error_page") continue;
		_errorPages[utils::stoi(i->getArg(0))] = i->getArg(1);
	}

	for (std::vector<AConfigBlock*>::iterator i = _blocks.begin(); i != _blocks.end(); ++i) {
		if (dynamic_cast<RouteBlock*>(*i))
			_routeBlocks.push_back(reinterpret_cast<RouteBlock*>(*i));
		(*i)->parseData();
	}

	_isParsed = true;
}

int ServerBlock::getPort() const {
	throwNotParsed();
	return _port;
}

const std::string &ServerBlock::getHost() const {
	throwNotParsed();
	return _host;
}

const std::vector<RouteBlock *> &ServerBlock::getRouteBlocks() const {
	throwNotParsed();
	return _routeBlocks;
}

int ServerBlock::getBodyLimit() const {
	throwNotParsed();
	return _bodyLimit;
}

const std::string &ServerBlock::getServerName() const {
	throwNotParsed();
	return _serverName;
}

RouteBlock *ServerBlock::findRoute(const std::string &path) {
	throwNotParsed();
	for (std::vector<RouteBlock*>::iterator route = _routeBlocks.begin(); route != _routeBlocks.end(); ++route) {
		if ((*route)->getLocation().match(path)) {
			return *route;
		}
	}
	return 0;
}

std::string	ServerBlock::getErrorPage(int code) const {
	std::map<int, std::string>::const_iterator it = _errorPages.find(code);
	if (it == _errorPages.end())
		return "";
	return it->second;
}
