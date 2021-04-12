//
// Created by pmerk on 22/03/2021.
//

#include "env/env.hpp"

using namespace CGIenv;

env::env() : _envp(0) { }

env::~env() {
	if (_envp) {
		for (int i = 0; _envp[i]; i++)
			delete [] _envp[i];
		delete [] _envp;
	}
}

char **env::getEnv() {
	return _envp;
}

void env::setEnv(char **envp) {
	_envp = envp;
}
