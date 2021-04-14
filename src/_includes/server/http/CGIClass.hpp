//
// Created by pmerku on 30/03/2021.
//

#ifndef CGICLASS_HPP
#define CGICLASS_HPP

#include "server/ServerTypes.hpp"
#include "server/http/HTTPClientData.hpp"
#include "env/env.hpp"
#include "utils/Uri.hpp"
#include <string>

namespace NotApache {

	// defines start are 128 because they arent reserved exit codes
	#define EXECVE_ERROR	129
	#define CLOSE_ERROR		130
	#define DUP2_ERROR		131
	#define CHDIR_ERROR		132
	#define GETCWD_ERROR	133
	#define MEMORY_ERROR	134

    class HTTPClient;

	class CgiClass {
    private:
        CGIenv::env         _envp;

	public:
		char 				**args;
		FD					pipefd[2];
		FD					bodyPipefd[2];
		bool 				body;
        pid_t               pid;
	    int                 status;

		HTTPClientRequest	request;
		HTTPClientResponse	response;

	    bool                hasExited;

		explicit CgiClass();
		~CgiClass();

        void generateENV(HTTPClient& client, const utils::Uri& uri, const std::string &rewrittenUrl);
	    CGIenv::env &getEnvp();

	    void closePipes(FD *pipefd0, FD *pipefd1, FD *bodyPipefd0, FD *bodyPipefd1);
	    void freeArgs();

	    class CGIException : public std::exception {
		public:
			virtual const char *what() const throw() {
				return "Failed to create CGI";
			}
		};
		class NotFound : public CGIException {
			public:
				virtual const char *what() const throw() { return "CGI not found"; }
		};
		class PipeFail : public CGIException {
			public:
				virtual const char *what() const throw() { return "Pipe fail"; }
		};
		class CloseFail : public CGIException {
			public:
				virtual const char *what() const throw() { return "Close fail"; }
		};
		class ForkFail : public CGIException {
			public:
				virtual const char *what() const throw() { return "Fork fail"; }
		};
		class CWDFail: public CGIException {
			public:
				virtual const char *what() const throw() { return "CWD fail"; }
		};
	};
} // namespace NotApache

#endif // CGICLASS_HPP
