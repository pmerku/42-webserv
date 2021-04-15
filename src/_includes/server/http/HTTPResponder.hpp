//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include <config/blocks/ServerBlock.hpp>
#include "server/global/GlobalLogger.hpp"
#include "server/http/HTTPClient.hpp"
#include <sys/stat.h>

#ifdef BUILD_APPLE
	#define STAT_TIME_FIELD st_mtimespec
#endif
#ifndef STAT_TIME_FIELD
	#define STAT_TIME_FIELD st_mtim
#endif

namespace NotApache {
	class HTTPResponder {
	public:
		static void generateResponse(HTTPClient &client);

		static void generateAssociatedResponse(HTTPClient &client);
		static void handleError(HTTPClient &client, int code);
		static bool checkCredentials(const std::vector<std::string>& authFile, const std::string& credentials);

		static void serveFile(HTTPClient &client);
		static void serveDirectory(HTTPClient &client, const struct ::stat &directoryStat);

		static void	prepareFile(HTTPClient &client, int code = 200);
		static void prepareFile(HTTPClient &client, const struct ::stat &buf, int code);

		static void	uploadFile(HTTPClient &client);
		static void	deleteFile(HTTPClient &client);

		static void handleProxy(HTTPClient &client);
		static void runCGI(HTTPClient& client, const std::string& cgi);
		static void	runJs(HTTPClient& client);
		static void	runBrainfuck(HTTPClient& client);

		class AuthHeader : public std::exception {
			public:
				virtual const char *what() const throw() { return "Invalid authentication header"; }
		};
	};
}

#endif //HTTPRESPONDER_HPP
