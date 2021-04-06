//
// Created by pmerku on 30/03/2021.
//

#ifndef CGICLASS_HPP
#define CGICLASS_HPP

#include "server/ServerTypes.hpp"
#include "server/http/HTTPClientData.hpp"
#include "utils/Uri.hpp"
#include <string>

namespace NotApache {

	class CgiClass {
	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;

		explicit CgiClass();
		~CgiClass();

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
