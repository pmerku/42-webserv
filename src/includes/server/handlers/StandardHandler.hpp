//
// Created by jelle on 3/3/2021.
//

#ifndef STANDARDHANDLER_HPP
#define STANDARDHANDLER_HPP

#include "server/handlers/AHandler.hpp"

namespace NotApache {
	class StandardHandler: public AHandler {
	private:
		static const int	_bufferSize;

	public:
		StandardHandler();

		virtual void	read(HTTPClient &client);
		virtual void	write(HTTPClient &client);

		void handleAssociatedRead(HTTPClient &client);

		void stopHandle(HTTPClient &client, bool shouldLock = true);
	};
}

#endif //STANDARDHANDLER_HPP
