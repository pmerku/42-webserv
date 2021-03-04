//
// Created by jelle on 3/4/2021.
//

#ifndef TERMINALCLIENT_HPP
#define TERMINALCLIENT_HPP

#include "server/Client.hpp"

namespace NotApache {

	class TerminalClient: public Client {
	public:
		TerminalClient(FD fd);

		virtual void	close();
	};

}

#endif //TERMINALCLIENT_HPP
