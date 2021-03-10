//
// Created by jelle on 3/3/2021.
//

#ifndef STANDARDHANDLER_HPP
#define STANDARDHANDLER_HPP

#include "server/handlers/AHandler.hpp"

namespace NotApache {
	class StandardHandler: public AHandler {
	private:
		static void			finish(Client &client);

	public:
		StandardHandler();

		virtual void	read(Client &client);
		virtual void	write(Client &client);

	};
}

#endif //STANDARDHANDLER_HPP
