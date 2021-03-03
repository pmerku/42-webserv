//
// Created by jelle on 3/3/2021.
//

#ifndef STANDARDHANDLER_HPP
#define STANDARDHANDLER_HPP

#include "server/AHandler.hpp"

namespace NotApache {
	class StandardHandler: public AHandler {
	public:
		StandardHandler();

		void	read(Client &client);
		void	write(Client &client);
	};
}

#endif //STANDARDHANDLER_HPP
