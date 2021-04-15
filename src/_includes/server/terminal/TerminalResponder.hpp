//
// Created by jelle on 3/13/2021.
//

#ifndef TERMINALRESPONDER_HPP
#define TERMINALRESPONDER_HPP

#include <string>


namespace NotApache {
	class Server;

	class TerminalResponder {
	private:
		static const std::string _help;
		
	public:
		void	respond(const std::string &str, Server* server);
	};

}

#endif //TERMINALRESPONDER_HPP
