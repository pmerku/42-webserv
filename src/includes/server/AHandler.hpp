//
// Created by jelle on 3/2/2021.
//

#ifndef AHANDLER_HPP
#define AHANDLER_HPP

#include "log/Loggable.hpp"
#include "server/Client.hpp"
#include "server/AParser.hpp"

namespace NotApache {
	///	Handles the read and write of a client connection
	class AHandler: public log::ILoggable {
	protected:
		std::vector<AParser*>	*_parsers;
	public:
		AHandler();

		virtual void	read(Client &client) = 0;
		virtual void	write(Client &client) = 0;

		void setParsers(std::vector<AParser*> *parsers);
	};
}

#endif //AHANDLER_HPP
