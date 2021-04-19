//
// Created by jelle on 4/19/2021.
//

#include "config/validators/DuplicateServerValidator.hpp"
#include "config/blocks/ServerBlock.hpp"
#include "config/blocks/RootBlock.hpp"

using namespace config;

DuplicateServerValidator::DuplicateServerValidator() : AConfigValidator(true) {}

void DuplicateServerValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	(void)line;
	const RootBlock &b = reinterpret_cast<const RootBlock&>(block);
	for (std::vector<ServerBlock *>::const_iterator i = b.getServerBlocks().begin(); i != b.getServerBlocks().end(); ++i) {
		for (std::vector<ServerBlock *>::const_iterator j = b.getServerBlocks().begin(); j != b.getServerBlocks().end(); ++j) {
			// check every serverblock against every server block
			if (i == j)
				continue;
			// if port is not the same, continue
			if ((*i)->getPort() != (*j)->getPort())
				continue;
			// if host is not the same, continue
			if ((*i)->getHost() != (*j)->getHost())
				continue;
			// if not the same server_name, continue
			if ((*i)->getServerName() != (*j)->getServerName())
				continue;
			throw DuplicateServerValidatorException(ConfigLine("hello world", block.getLineNumber()), &block);
		}
	}
}
