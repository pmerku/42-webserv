//
// Created by jelle on 3/2/2021.
//

#ifndef ALISTENER_HPP
#define ALISTENER_HPP

#include "log/Loggable.hpp"
#include "server/FileDescriptor.hpp"
#include "server/Client.hpp"

namespace NotApache {
	///	Generic listener that can handle incoming connections
	class AListener: public log::ILoggable {
	public:
		virtual ~AListener() {};

		virtual FD		getFD() = 0;
		virtual void	start() = 0;
		virtual Client	*acceptClient() = 0;
	};
}

#endif //ALISTENER_HPP
