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
#include "config/validators/FileNameValidator.hpp"
#include "config/validators/StartsWithValidator.hpp"
#include "config/validators/RegexCompiler.hpp"
#include "config/validators/PluginValidator.hpp"
#include "config/validators/UrlValidator.hpp"

using namespace config;

const AConfigBlock::validatorsMapType	RouteBlock::_validators =
		ConfigValidatorBuilder()
		.addKey("location", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new StartsWithValidator(0, '/'))
		  .add(new RegexCompiler(0))
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
		.addKey("index", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new FileNameValidator(0))
		  .add(new Unique())
		  .build())
		.addKey("cgi", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .add(new RequiredKey("cgi_ext", true))
		  .add(new IsFile(0))
		  .build())
	  	.addKey("cgi_ext", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new FileNameValidator(0))
		  .add(new StartsWithValidator(0, '.', true))
		  .add(new RequiredKey("cgi", true))
		  .add(new Unique())
		  .build())
		.addKey("save_uploads", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .add(new IsDirectory(0))
		  .build())
		.addKey("use_plugin", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new PluginValidator(0))
		  .build())
		.addKey("proxy_url", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new UrlValidator(0))
		  .add(new Unique())
		  .build())
		.addKey("auth_basic", ConfigValidatorListBuilder()
		  .add(new ArgumentLength(1))
		  .add(new Unique())
		  .build())
		.addKey("authorized", ConfigValidatorListBuilder()
		  .add(new Unique())
		  .build())
		.addKey("accept_language", ConfigValidatorListBuilder()
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

RouteBlock::RouteBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): AConfigBlock(line, lineNumber, parent), _location("/"), _directoryListing() {}

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

void RouteBlock::parseData() {
	_location = regex::Regex(getKey("location")->getArg(0));
	_root = "";
	_directoryListing = false;
	_index = "";
	_saveUploads = "";
	_cgiExt = "";
	_cgi = "";
	_authBasic = "";
	_plugins.clear();
	_allowedMethods.clear();
	_allowedMethods.push_back("GET");
	_allowedMethods.push_back("POST");
	_allowedMethods.push_back("OPTIONS");
	_allowedMethods.push_back("HEAD");

	if (hasKey("root"))
		_root = getKey("root")->getArg(0);
	if (hasKey("directory_listing"))
		_directoryListing = getKey("directory_listing")->getArg(0) == "true";
	if (hasKey("index"))
		_index = getKey("index")->getArg(0);
	if (hasKey("save_uploads"))
		_saveUploads = getKey("save_uploads")->getArg(0);
	if (hasKey("proxy_url"))
		_proxyUrl = UrlValidator::parseUrl(getKey("proxy_url")->getArg(0));
	if (hasKey("cgi"))
		_cgi = getKey("cgi")->getArg(0);
	if (hasKey("cgi_ext"))
		_cgiExt = getKey("cgi_ext")->getArg(0);
	if (hasKey("allowed_methods")) {
		_allowedMethods.clear();
		for (ConfigLine::arg_size i = 0; i < getKey("allowed_methods")->getArgLength(); i++)
			_allowedMethods.push_back(getKey("allowed_methods")->getArg(i));
	}
	if (hasKey("auth_basic"))
		_authBasic = getKey("auth_basic")->getArg(0);
	if (hasKey("authorized")) {
		for (ConfigLine::arg_size i = 0; i < getKey("authorized")->getArgLength(); i++)
			_authorized.push_back(getKey("authorized")->getArg(i));
	}
	if (hasKey("accept_language")) {
		for (ConfigLine::arg_size i = 0; i < getKey("accept_language")->getArgLength(); i++)
			_acceptLanguage.push_back(getKey("accept_language")->getArg(i));
	}
	for (std::vector<ConfigLine>::const_iterator i = _lines.begin(); i != _lines.end(); ++i) {
		if (i->getKey() != "use_plugin") continue;
		_plugins.push_back(i->getArg(0));
	}
	
	_isParsed = true;
}

regex::Regex &RouteBlock::getLocation() {
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

const std::string &RouteBlock::getCgiExt() const {
	throwNotParsed();
	return _cgiExt;
}

const std::string &RouteBlock::getSaveUploads() const {
	throwNotParsed();
	return _saveUploads;
}

const UrlValidator::urlParsed &RouteBlock::getProxyUrl() const {
	throwNotParsed();
	return _proxyUrl;
}

const std::string &RouteBlock::getAuthBasic() const {
	throwNotParsed();
	return _authBasic;
}

const std::vector<std::string> &RouteBlock::getAuthorized() const {
	throwNotParsed();
	return _authorized;
}

const std::vector<std::string> &RouteBlock::getAcceptLanguage() const {
	throwNotParsed();
	return _acceptLanguage;
}

bool RouteBlock::isAllowedMethod(const std::string &method) const {
	throwNotParsed();
	for (std::vector<std::string>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it) {
		if (*it == method)
			return true;
	}
	return false;
}

bool RouteBlock::shouldDoFile() const {
	throwNotParsed();
	return _proxyUrl.protocol.empty();
}

bool RouteBlock::shouldDoCgi() const {
	throwNotParsed();
	return !_cgi.empty();
}
