//
// Created by jelle on 3/3/2021.
//

#ifndef STANDARDHANDLER_HPP
#define STANDARDHANDLER_HPP

#include "rewrite-server/handlers/AHandler.hpp"

namespace NotApacheRewrite {
	class StandardHandler: public AHandler {
	public:
		StandardHandler();

		virtual void	read(HTTPClient &client);
		virtual void	write(HTTPClient &client);

	};
}

#endif //STANDARDHANDLER_HPP
