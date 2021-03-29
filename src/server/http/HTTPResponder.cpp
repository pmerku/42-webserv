//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "utils/intToString.hpp"
#include "utils/strdup.hpp"
#include "utils/ErrorThrow.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/http/HTTPParser.hpp"
#include "env/ENVBuilder.hpp"

#include <sys/stat.h>
#include <unistd.h>

using namespace NotApache;

void HTTPResponder::generateResponse(HTTPClient &client) {
	std::string str = "lorem ipsum dolor sit amet";
	try {
		client.data.response.setResponse(
				ResponseBuilder("HTTP/1.1")
				.setStatus(400)
				.setHeader("Server", "Not-Apache")
				.setDate()
			 	.setHeader("Connection", "Close")
			 	.setBody(str, str.length())
			 	.build()
			 	);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("Response could not be built: ") + e.what());
	}

	try {
		CGIenv::env envp;
		envp.setEnv(CGIenv::ENVBuilder()
			.AUTH_TYPE(client.data.request._headers["AUTHORIZATION"])
			.CONTENT_LENGTH(utils::intToString(client.data.request._body.length()))
			.CONTENT_TYPE(client.data.request._headers["CONTENT_TYPE"])
			.GATEWAY_INTERFACE("CGI/1.1")
			//.PATH_INFO()
			//.PATH_TRANSLATED()
			//.QUERY_STRING()
			//.REMOTE_ADDR()
			//.REMOTE_IDENT()
			.REMOTE_USER(client.data.request._headers["REMOTE-USER"])
			.REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request._method)->second)
			.REQUEST_URI(client.data.request._uri)
			//.SCRIPT_NAME()
			//.SERVER_NAME()
			//.SERVER_PORT()
			.SERVER_PROTOCOL("HTTP/1.1")
			.SERVER_SOFTWARE("HTTP 1.1")
			.build()
		);
		for (int i = 0; envp.getEnv()[i]; i++)
			std::cout << envp.getEnv()[i] << std::endl;
		runCGI(client, envp);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("ENV could not be built: ") + e.what());
	}

}

void	HTTPResponder::runCGI(HTTPClient &client, CGIenv::env& envp) {  // TODO error handling
	int		pid;
	FD		pipefd[2];
	FD		bodyPipefd[2];
	struct stat sb;
	bool 	body = false;

	char** args = new char *[2]();
	args[0] = utils::strdup(client.data.request._uri.c_str());

	if (::stat(args[0], &sb) == -1) {
		globalLogger.logItem(logger::ERROR, "cgi not found");
		//ERROR_THROW "file not found";
	}

	if (::pipe(pipefd) == -1) {
		globalLogger.logItem(logger::ERROR, "pipe failed");
		return ;
	}
	client.data.response._fd = pipefd[0];

	if (client.data.request._body.length()) {
		std::cout << "test\n";
		::pipe(bodyPipefd);
		client.data.response._bodyfd = bodyPipefd[0];
		body = true;
	}

	pid = ::fork();
	if (pid == -1) {
		globalLogger.logItem(logger::ERROR, "fork failed");
		return ;
	}
	if (!pid)
	{
		if (!body)
			::close(STDIN_FILENO);
		else
			::dup2(STDIN_FILENO, bodyPipefd[0]);
		if (::dup2(STDOUT_FILENO, pipefd[1]) == -1) {
			globalLogger.logItem(logger::ERROR, "dup2 failed");
			return ;
		}
		::close(pipefd[0]);
		// ::close(pipefd[1]);
		if (::execve(args[0], args, envp.getEnv()) == -1) {
			globalLogger.logItem(logger::ERROR, "execve failed");
			return ;
		}
	}
	::close(pipefd[1]);
}
