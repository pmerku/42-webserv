//
// Created by jelle on 3/26/2021.
//

#include "config/validators/IsFile.hpp"
#include <sys/stat.h>
#include "utils/ErrorThrow.hpp"

using namespace config;

IsFile::IsFile(int arg): AConfigValidator(false), _arg(arg) {}

void IsFile::test(const ConfigLine &line, const AConfigBlock &block) const {
	struct stat	buf = {};

	// arg doesnt exist
	if (line.getArgLength() < (ConfigLine::arg_size)_arg)
		ERROR_THROW(IsFileException(line, &block));

	// invalid path/no perms
	if (::stat(line.getArg(_arg).c_str(), &buf) == -1)
		ERROR_THROW(IsFileException(line, &block));

	// check if regular file
	if (!S_ISREG(buf.st_mode))
		ERROR_THROW(IsFileException(line, &block));
}
