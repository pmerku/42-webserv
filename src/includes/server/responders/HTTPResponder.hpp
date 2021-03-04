//
// Created by jelle on 3/4/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include "server/responders/AResponder.hpp"

namespace NotApache {

	class HTTPResponder: public AResponder {
	public:
		HTTPResponder();

		std::string	generateResponse(Client &client) const;
		std::string	generateParseError(Client &client) const;
	};

}

#endif //HTTPRESPONDER_HPP
