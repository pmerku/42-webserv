//
// Created by jelle on 3/4/2021.
//

#ifndef TERMINAL_RESPONDER_HPP
#define TERMINAL_RESPONDER_HPP

#include "server/responders/AResponder.hpp"

namespace NotApache {

	class TerminalResponder: public AResponder {
	public:
		TerminalResponder();

		std::string	generateResponse(Client &client) const;
		std::string	generateParseError(Client &client) const;
	};

}

#endif //TERMINAL_RESPONDER_HPP
