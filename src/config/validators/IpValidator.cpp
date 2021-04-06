//
// Created by jelle on 3/26/2021.
//

#include "config/validators/IpValidator.hpp"
#include "utils/isDigit.hpp"
#include "utils/atoi.hpp"
#include "utils/ErrorThrow.hpp"

using namespace config;

IpValidator::IpValidator(int arg): AConfigValidator(false), _arg(arg) {}

void IpValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (!isValidIp(line.getArg(_arg)))
		ERROR_THROW(IpValidatorException(line, &block));
}

bool IpValidator::isValidIp(const std::string &str) {
	std::string	temp = str;

	// check for valid chars
	for (std::string::size_type i = 0; i < temp.length(); ++i) {
		if (!utils::isDigit(temp[i]) && temp[i] != '.')
			return false;
	}

	// check if in right order
	int parts = 0;
	while (true) {
		std::string::size_type pos = temp.find('.');
		std::string part = temp.substr(0, pos);

		// invalid length
		if (part.length() == 0 || part.length() > 3)
			return false;

		// part count
		parts++;
		if (parts > 4)
			return false;

		// invalid number
		int num = utils::atoi(part.c_str());
		if (num < 0 || num > 255)
			return false;

		// next iteration
		if (pos != std::string::npos) {
			temp = temp.substr(pos+1);
			// dot at end of string
			if (temp.length() == 0)
				return false;
		}
		else break;
	}

	// part count
	if (parts != 4)
		return false;
	return true;
}
