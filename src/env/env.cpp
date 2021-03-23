//
// Created by pmerk on 22/03/2021.
//

#include "env/env.hpp"
#include <cstdlib>

using namespace CGIenv;

env::env() : _envp(NULL) { }

env::~env() {
	if (_envp) {
		for (int i = 0; _envp[i]; i++)
			free(_envp[i]);
		free(_envp);
	}
}

char **env::getEnv() const {
	return _envp;
}

void env::setEnv(char **envp) {
	_envp = envp;
}
