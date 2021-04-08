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
		enum IOReturn {
			IO_EOF,
			IO_ERROR,
			SUCCESS
		};

		IOReturn		doRead(FD fd, utils::DataList &readable);
		IOReturn		doWrite(FD fd, HTTPClientRequest &writable, utils::DataList &data);

		void handleAssociatedRead(HTTPClient &client);
		void handleAssociatedWrite(HTTPClient &client);

		void stopHandle(HTTPClient &client, bool shouldLock = true);

	public:
		StandardHandler();


		virtual void	read(HTTPClient &client);
		virtual void	write(HTTPClient &client);

	};
}

#endif //STANDARDHANDLER_HPP
