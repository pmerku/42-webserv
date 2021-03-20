//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include "server/http/HTTPClient.hpp"
#include "server/http/response/AResponse.hpp"

namespace NotApache {

	class HTTPResponder {
	private:
		response::AResponse _response;

	public:
		void	generateResponse(HTTPClient &client);
	};

}

#endif //HTTPRESPONDER_HPP
