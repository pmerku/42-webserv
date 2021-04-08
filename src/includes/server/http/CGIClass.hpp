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

#define EXECVE_ERROR 1
#define CLOSE_ERROR 2
#define DUP2_ERROR 3

    class HTTPClient;

	class CgiClass {
    private:
        CGIenv::env         _envp;

	public:
        pid_t               pid;
	    int                 status;

		HTTPClientRequest	request;
		HTTPClientResponse	response;

	    bool                hasExited;

		explicit CgiClass();
		~CgiClass();

        void generateENV(HTTPClient& client, const utils::Uri& uri, const std::string &filePath, const std::string &execPath);
	    const CGIenv::env &getEnvp() const;

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
