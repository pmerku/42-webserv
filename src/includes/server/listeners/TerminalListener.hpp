//
// Created by jelle on 3/3/2021.
//

#ifndef TERMINALLISTENER_HPP
#define TERMINALLISTENER_HPP

#include <netinet/in.h>
#include <exception>
#include "server/TerminalClient.hpp"
#include "server/listeners/AListener.hpp"
#include "log/Loggable.hpp"

namespace NotApache {

	///	Listener for accepting terminal commands
	class TerminalListener: public AListener {
	private:
		Client	*_client;

	public:
		TerminalListener();
		~TerminalListener();

		FD		getFD();
		void	start();
		Client	*acceptClient();
	};
}

#endif //TERMINALLISTENER_HPP
