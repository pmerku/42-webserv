//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "utils/strdup.hpp"
#include <unistd.h>
#include <cstdlib>

using namespace NotApache;

void	HTTPResponder::runCGI(HTTPClient& client, const std::string& cgiPath) {
	struct stat	sb;
	client.cgi = new CgiClass;

	char curCwd[1024];
	if (::getcwd(curCwd, 1023) == NULL)
		ERROR_THROW(CgiClass::NotFound());
	utils::Uri curCwdUri(curCwd);
	if (cgiPath[0] == '/')
		curCwdUri = utils::Uri(cgiPath);
	else
		curCwdUri.appendPath(cgiPath, true);

	client.cgi->generateENV(client, client.data.request.data.uri, client.rewrittenUrl);

	client.cgi->args[0] = utils::strdup(curCwdUri.path);
	client.cgi->args[1] = utils::strdup(client.rewrittenUrl.substr(1));

	if (::stat(client.cgi->args[0], &sb) == -1) {
		client.cgi->freeArgs();
		ERROR_THROW(CgiClass::NotFound());
	}

	if (::pipe(client.cgi->pipefd) == -1) {
		client.cgi->freeArgs();
		ERROR_THROW(CgiClass::PipeFail());
	}
	if (::pipe(client.cgi->bodyPipefd) == -1) {
		client.cgi->freeArgs();
		client.cgi->closePipes(&client.cgi->pipefd[0], &client.cgi->pipefd[1], NULL, NULL);
		ERROR_THROW(CgiClass::PipeFail());
	}

	long int bodyLen = client.data.request.data.body.size();
	if (bodyLen > 0)
		client.cgi->body = true;

	client.cgi->pid = ::fork();
	client.cgi->hasExited = false;
	if (client.cgi->pid == -1) {
		client.cgi->freeArgs();
		client.cgi->closePipes(&client.cgi->pipefd[0], &client.cgi->pipefd[1], &client.cgi->bodyPipefd[0], &client.cgi->bodyPipefd[1]);
		ERROR_THROW(CgiClass::ForkFail());
	}
	if (!client.cgi->pid) {
		char buf[1024];

		// CHILD PROCESS
		// change directory to document root
		if (::chdir(client.routeBlock->getRoot().c_str()) == -1)
			::exit(CHDIR_ERROR);

		if (::getcwd(buf, 1023) == NULL)
			::exit(GETCWD_ERROR);

		try {
			std::string documentRoot = buf;
			std::string pathTranslated = documentRoot + client.rewrittenUrl;
			for (char **envp = client.cgi->getEnvp().getEnv(); *envp != NULL; envp++) {
				std::string envStr = *envp;
				if (envStr.find("DOCUMENT_ROOT=") == 0) {
					envStr += documentRoot;
					delete [] *envp;
					*envp = utils::strdup(const_cast<char *>(envStr.c_str()));
					continue;
				}
				if (envStr.find("PATH_TRANSLATED=") == 0) {
					envStr += pathTranslated;
					delete [] *envp;
					*envp = utils::strdup(const_cast<char *>(envStr.c_str()));
					continue;
				}
			}
		} catch (std::exception &e) {
			::exit(MEMORY_ERROR);
		}

		// set body pipes
		if (::dup2(client.cgi->bodyPipefd[0], STDIN_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(client.cgi->bodyPipefd[0]) == -1)
			::exit(CLOSE_ERROR);
		if (::close(client.cgi->bodyPipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// set output pipes
		if (::dup2(client.cgi->pipefd[1], STDOUT_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(client.cgi->pipefd[0]) == -1 || ::close(client.cgi->pipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// run cgi
		::execve(client.cgi->args[0], client.cgi->args, client.cgi->getEnvp().getEnv());
		::exit(EXECVE_ERROR);
	}

	// CURRENT PROCESS
	client.cgi->freeArgs();
	try {
		FD *bodyPipe = client.cgi->body ? 0 : &client.cgi->bodyPipefd[1];
		client.cgi->closePipes(0, &client.cgi->pipefd[1], &client.cgi->bodyPipefd[0], bodyPipe);
	} catch (std::exception &e) {
		FD *bodyPipe = client.cgi->body ? &client.cgi->bodyPipefd[1] : 0;
		client.cgi->closePipes(&client.cgi->pipefd[0], 0, 0, bodyPipe);
		throw;
	}

	client.addAssociatedFd(client.cgi->pipefd[0]);
	if (client.cgi->body)
		client.addAssociatedFd(client.cgi->bodyPipefd[1], associatedFD::WRITE);

	client.responseState = CGI;
	client.connectionState = ASSOCIATED_FD;
}
