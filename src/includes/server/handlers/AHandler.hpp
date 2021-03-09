//
// Created by jelle on 3/2/2021.
//

#ifndef AHANDLER_HPP
#define AHANDLER_HPP

#include "log/Loggable.hpp"
#include "server/Client.hpp"
#include "server/parsers/AParser.hpp"
#include "server/responders/AResponder.hpp"

namespace NotApache {
	///	Handles the read and write of a client connection
	class AHandler: public logger::ILoggable {
	protected:
		std::vector<AParser*>		*_parsers;
		std::vector<AResponder*>	*_responders;

	public:
		AHandler();
		virtual ~AHandler();

		void	respond(Client &client);

		virtual void	read(Client &client) = 0;
		virtual void	write(Client &client) = 0;

		void setParsers(std::vector<AParser*> *parsers);
		void setResponders(std::vector<AResponder*> *responders);
	};
}

#endif //AHANDLER_HPP
