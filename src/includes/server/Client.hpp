//
// Created by jelle on 3/2/2021.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "server/ServerTypes.hpp"
#include "server/parsers/AParser.hpp"

namespace NotApache {
	///	Client connection, holds FD and the current data of the connection
	class Client {
	private:
		FD				_fd;
		ClientTypes		_type;
		ClientStates	_state;
		std::string		_dataType;
		std::string		_request;
		std::string		_response;
		std::size_t		_responseIndex;
		ResponseStates	_responseState;

	public:
		Client(FD fd, ClientTypes type = CONNECTION);
		virtual ~Client();

		FD				getFD() const;
		ClientStates	getState() const;
		std::string		getRequest() const;
		ClientTypes		getType() const;
		std::string		getDataType() const;
		std::string		getResponse() const;
		std::size_t		getResponseIndex() const;
		ResponseStates	getResponseState() const;

		void	setFD(FD fd);
		void 	setState(ClientStates state);
		void 	setDataType(const std::string &str);
		void 	setResponseState(ResponseStates state);

		void	appendRequest(const std::string &str);
		void	setRequest(const std::string &str);

		void	appendResponse(const std::string &str);
		void	setResponse(const std::string &str);
		void 	setResponseIndex(std::size_t i);

		virtual void	close();

	};
}


#endif //CLIENT_HPP
