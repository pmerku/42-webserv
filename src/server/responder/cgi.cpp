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
	if (::pipe(client.cgi->bodyPipefd)) {
		client.cgi->freeArgs();
		client.cgi->closePipes(client.cgi->pipefd[0], client.cgi->pipefd[]);
		ERROR_THROW(CgiClass::PipeFail());
	}

	long int bodyLen = client.data.request.data.isChunked ? client.data.request.data.chunkedData.size() : client.data.request.data.data.size();
	if (bodyLen > 0)
		client.cgi->body = true;

	client.cgi->pid = ::fork();
	client.cgi->hasExited = false;
	if (client.cgi->pid == -1)
		ERROR_THROW(CgiClass::ForkFail());
	if (!client.cgi->pid) {
		char buf[1024];

		// CHILD PROCESS
		// change directory to document root
		// TODO is this is the right dir?
		// TODO check with -> https://www.w3schools.com/php/php_includes.asp
		// TODO use DOCUMENT_ROOT
		if (::chdir(client.routeBlock->getRoot().c_str()) == -1)
			::exit(CHDIR_ERROR);

		if (::getcwd(buf, 1023) == NULL)
			::exit(GETCWD_ERROR);

		try {
			std::string *pathTranslated = new std::string(buf);
			*pathTranslated += client.rewrittenUrl;
			pathTranslated->insert(0, "PATH_TRANSLATED=");
			for (char **envp = client.cgi->getEnvp().getEnv(); *envp != NULL; envp++) {
				std::cerr << *envp << std::endl;
				std::string envStr = *envp;
				if (envStr.find("PATH_TRANSLATED=") == 0) {
					delete [] *envp;
					*envp = const_cast<char *>(pathTranslated->c_str());
					break;
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

	client.cgi->freeArgs();

	// CURRENT PROCESS
	int fails = 0;
	fails += ::close(client.cgi->pipefd[1]) == -1;
	fails += ::close(client.cgi->bodyPipefd[0]) == -1;
	if (!client.cgi->body)
		fails += ::close(client.cgi->bodyPipefd[1]) == -1;
	if (fails > 0) {
		client.cgi->closePipes(&client.cgi->pipefd[0], &client.cgi->pipefd[1], &client.cgi->bodyPipefd[0], &client.cgi->bodyPipefd[1]);
		ERROR_THROW(CgiClass::CloseFail());
	}

	client.addAssociatedFd(client.cgi->pipefd[0]);
	if (client.cgi->body)
		client.addAssociatedFd(client.cgi->bodyPipefd[1], associatedFD::WRITE);

	client.responseState = CGI;
	client.connectionState = ASSOCIATED_FD;
}
