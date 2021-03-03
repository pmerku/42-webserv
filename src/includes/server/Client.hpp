//
// Created by jelle on 3/2/2021.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "server/FileDescriptor.hpp"

namespace NotApache {
	enum ClientStates {
		READING,
		WRITING,
		CLOSED,
	};

	enum ClientTypes {
		TERMINAL,
		CONNECTION
	};

	///	Client connection, holds FD and the current data of the connection
	class Client {
	private:
		FD				_fd;
		ClientTypes		_type;
		ClientStates	_state;
		std::string		_request;

	public:
		Client(FD fd, ClientTypes type = CONNECTION);

		FD				getFD() const;
		ClientStates	getState() const;
		std::string		getRequest() const;
		ClientTypes		getType() const;

		void	setFD(FD fd);
		void 	setState(ClientStates state);
		void	appendRequest(const std::string &str);
		void	setRequest(const std::string &str);
	};
}


#endif //CLIENT_HPP
