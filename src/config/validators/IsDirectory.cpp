//
// Created by jelle on 3/26/2021.
//

#include "config/validators/IsDirectory.hpp"
#include <sys/stat.h>

using namespace config;

IsDirectory::IsDirectory(int arg): AConfigValidator(false), _arg(arg) {}

void IsDirectory::test(const ConfigLine &line, const AConfigBlock &block) const {
	struct stat	buf = {};

	// arg doesnt exist
	if (line.getArgLength() < (ConfigLine::arg_size)_arg)
		throw IsDirectoryException(line, &block);

	// invalid path/no perms
	if (::stat(line.getArg(_arg).c_str(), &buf) == -1)
		throw IsDirectoryException(line, &block);

	// check if regular file
	if (!S_ISDIR(buf.st_mode))
		throw IsDirectoryException(line, &block);
}
