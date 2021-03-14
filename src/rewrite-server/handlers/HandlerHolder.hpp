//
// Created by jelle on 3/13/2021.
//

#ifndef HANDLERHOLDER_HPP
#define HANDLERHOLDER_HPP

#include "rewrite-server/handlers/AHandler.hpp"
#include "rewrite-server/http/HTTPClient.hpp"
#include <vector>

namespace NotApacheRewrite {

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
