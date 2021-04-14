//
// Created by jelle on 3/13/2021.
//

#ifndef HANDLERHOLDER_HPP
#define HANDLERHOLDER_HPP

#include "server/handlers/AHandler.hpp"
#include "server/http/HTTPClient.hpp"
#include <vector>

namespace NotApache {

	class HandlerHolder {
	public:
		enum handlerActions {
			READ,
			WRITE
		};

	private:
		typedef std::vector<AHandler *>	container;
		container::size_type			_roundRobin;
		container::size_type			_size;
		container						_handlers;

	public:
		/// will destruct all received handlers
		HandlerHolder();
		virtual ~HandlerHolder();

		void	addHandler(AHandler *handler);
		void 	handleClient(HTTPClient &client, handlerActions action);
	};

}

#endif //HANDLERHOLDER_HPP
