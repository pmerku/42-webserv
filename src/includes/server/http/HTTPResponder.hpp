//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include "server/http/HTTPClient.hpp"
#include "env/env.hpp"

namespace NotApache {
	class HTTPResponder {
	public:
		static void		generateResponse(HTTPClient &client);
		static void		runCGI(HTTPClient &client, CGIenv::env& envp);
	};

}

#endif //HTTPRESPONDER_HPP
