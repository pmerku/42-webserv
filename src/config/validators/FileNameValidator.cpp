//
// Created by jelle on 3/30/2021.
//

#include "config/validators/FileNameValidator.hpp"
#include "utils/isDigit.hpp"

using namespace config;

FileNameValidator::FileNameValidator(int argnum): AConfigValidator(true), _argnum(argnum) {}

void FileNameValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	if (line.getArgLength() <= (unsigned long)_argnum)
		ERROR_THROW(FileNameValidatorException(line, &block));
	const std::string &str = line.getArg(_argnum);
	for (std::string::size_type i = 0; i < str.length(); i++) {
		// linux filenames may not have a slash or NULL in a filename
		if (str[i] == '/' || str[i] == '\0')
			ERROR_THROW(FileNameValidatorException(line, &block));
	}
}
