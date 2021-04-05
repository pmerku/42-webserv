//
// Created by jelle on 3/9/2021.
//

#include "config/validators/UrlValidator.hpp"
#include "config/validators/IpValidator.hpp"
#include "utils/stoi.hpp"
#include "utils/toUpper.hpp"

using namespace config;

UrlValidator::UrlValidator(int argnum): AConfigValidator(false), _argnum(argnum) {}

void UrlValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	try {
		UrlValidator::parseUrl(line.getArg(_argnum));
	} catch (UrlParseProtocolError &e) {
		ERROR_THROW(UrlValidatorProtocolException(line, &block));
	} catch (std::exception &e) {
		ERROR_THROW(UrlValidatorInvalidException(line, &block));
	}
}

UrlValidator::urlParsed UrlValidator::parseUrl(const std::string &str) {
	urlParsed	out = {};
	out.port = 80;
	out.protocol = "HTTP";

	std::string::size_type	ipPos = 0;

	// protocol check
	std::string::size_type	protocolSeperator = str.find("://");
	if (protocolSeperator != std::string::npos) {
		std::string	protocol = str.substr(0, protocolSeperator);
		utils::toUpper(protocol);
		if (protocol != "HTTP")
			ERROR_THROW(UrlParseProtocolError());
		out.protocol = protocol;
		ipPos = protocolSeperator + 3;
	}

	// check ip address
	std::string::size_type endIpPos = str.find(':', ipPos);
	out.ip = str.substr(ipPos, endIpPos - ipPos);
	if (out.ip.empty())
		ERROR_THROW(UrlParseError());
	if (!IpValidator::isValidIp(out.ip))
		ERROR_THROW(UrlParseError());

	// port parsing
	if (endIpPos != std::string::npos && str[endIpPos] == ':') {
		endIpPos++;
		std::string::size_type endPortPos = str.find_first_not_of("0123456789", endIpPos);
		std::string	portStr = str.substr(endIpPos, endPortPos);
		endIpPos = endPortPos;

		if (portStr.empty())
			ERROR_THROW(UrlParseError());
		out.port = utils::stoi(portStr);
	}

	if (endIpPos != std::string::npos)
		ERROR_THROW(UrlParseError());
	return out;
}
