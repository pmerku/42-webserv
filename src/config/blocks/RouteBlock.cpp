//
// Created by jelle on 3/8/2021.
//

#include "config/blocks/RouteBlock.hpp"
#include "config/ConfigValidatorBuilder.hpp"
#include "config/validators/ArgumentLength.hpp"
#include "config/validators/RequiredKey.hpp"
#include "config/validators/Unique.hpp"
#include "config/validators/MutuallyExclusive.hpp"
#include "config/validators/BooleanValidator.hpp"
#include "config/validators/HTTPMethodValidator.hpp"
#include "config/validators/IsDirectory.hpp"
#include "config/validators/IsFile.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	RouteBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("location", ConfigValidatorListBuilder() // TODO location validator (start with slash) + regex validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("allowed_methods", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(0, 9))
		  .add(new HTTPMethodValidator())
		  .add(new Unique())
		  .build())
		.addKey("root", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .add(new IsDirectory(0))
		  .build())
		.addKey("directory_listing", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new BooleanValidator(0))
		  .add(new Unique())
		  .build())
		.addKey("index", ConfigValidatorListBuilder() // TODO filename validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("cgi", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .add(new IsFile(0))
		  .build())
		.addKey("save_uploads", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .add(new IsDirectory(0))
		  .build())
		.addKey("use_plugin", ConfigValidatorListBuilder() // TODO plugin validator
		  .add(new ArgumentLength(1))
		  .build())
		.addKey("proxy_url", ConfigValidatorListBuilder() // TODO url validator
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.build();

const AConfigBlock::validatorListType 	RouteBlock::_blockValidators =
		ConfigValidatorListBuilder()
		.add(new RequiredKey("location"))
		.add(new MutuallyExclusive("proxy_url", "root"))
		.build();

const std::string 						RouteBlock::_allowedBlocks[] = { "" };

const AConfigBlock::validatorsMapType	&RouteBlock::getValidators() const {
	return RouteBlock::_validators;
}

const std::string						*RouteBlock::getAllowedBlocks() const {
	return RouteBlock::_allowedBlocks;
}

RouteBlock::RouteBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent) {}

std::string RouteBlock::getType() const {
	return "route";
}

const AConfigBlock::validatorListType &RouteBlock::getBlockValidators() const {
	return _blockValidators;
}

void	RouteBlock::cleanup() {
	for (validatorsMapType::const_iterator i = _validators.begin(); i != _validators.end(); ++i) {
		for (validatorListType::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
			delete *j;
		}
	}
	for (validatorListType::const_iterator i = _blockValidators.begin(); i != _blockValidators.end(); ++i) {
		delete *i;
	}
}

// TODO allowed methods parsing
// TODO plugin parsing
void RouteBlock::parseData() {
	_location = getKey("location")->getArg(0);
	_root = "";
	_directoryListing = false;
	_index = "index.html";
	_saveUploads = "";
	_proxyUrl = "";

	if (hasKey("root"))
		_root = getKey("root")->getArg(0);
	if (hasKey("directory_listing"))
		_directoryListing = getKey("directory_listing")->getArg(0) == "true";
	if (hasKey("index"))
		_index = getKey("index")->getArg(0);
	if (hasKey("save_uploads"))
		_saveUploads = getKey("save_uploads")->getArg(0);
	if (hasKey("proxy_url"))
		_proxyUrl = getKey("proxy_url")->getArg(0);
}

const std::string &RouteBlock::getLocation() const {
	throwNotParsed();
	return _location;
}

const std::vector<std::string> &RouteBlock::getAllowedMethods() const {
	throwNotParsed();
	return _allowedMethods;
}

const std::string &RouteBlock::getRoot() const {
	throwNotParsed();
	return _root;
}

bool RouteBlock::isDirectoryListing() const {
	throwNotParsed();
	return _directoryListing;
}

const std::string &RouteBlock::getIndex() const {
	throwNotParsed();
	return _index;
}

const std::string &RouteBlock::getCgi() const {
	throwNotParsed();
	return _cgi;
}

const std::string &RouteBlock::getSaveUploads() const {
	throwNotParsed();
	return _saveUploads;
}

const std::string &RouteBlock::getProxyUrl() const {
	throwNotParsed();
	return _proxyUrl;
}

bool RouteBlock::shouldDoFile() const {
	throwNotParsed();
	return _proxyUrl.empty();
}

bool RouteBlock::shouldDoCgi() const {
	throwNotParsed();
	return !_cgi.empty();
}
