//
// Created by pmerku on 30/03/2021.
//

#ifndef PROXY_HPP
#define PROXY_HPP

#include "server/ServerTypes.hpp"
#include "server/http/HTTPClientData.hpp"
#include "utils/Uri.hpp"
#include <string>

namespace NotApache {

	class Proxy {
	private:
		FD          _socket;
		std::string	_url;
		int 		_port;
		utils::Uri 	_uri;

	public:
		HTTPClientRequest	request;
		HTTPClientResponse	response;

		explicit Proxy(const std::string &url, int port = 80);
		~Proxy();

		void	createConnection() const;
		FD		getSocket() const;
		void 		setURI(const utils::Uri &uri);
		utils::Uri	getURI() const;

		class ProxyException : public std::exception {
		public:
			const char *what() const throw() {
				return "Failed to create proxy";
			}
		};

		class SocketException : public ProxyException {
		public:
			const char *what() const throw() {
				return "Cannot create socket";
			}
		};

		class ConnectionException : public ProxyException {
		public:
			const char *what() const throw() {
				return "Cannot connect to socket";
			}
		};
	};

} // namespace NotApache

#endif // PROXY_HPP
