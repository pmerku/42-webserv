//
// Created by jelle on 3/4/2021.
//

#ifndef ARESPONDER_HPP
#define ARESPONDER_HPP

#include <string>
#include "server/ServerTypes.hpp"
#include "server/Client.hpp"

namespace NotApache {
	class AResponder {
		const std::string	_type;

	public:
		AResponder(const std::string &type);
		virtual ~AResponder();

		virtual std::string generateResponse(Client &client) const = 0;
		virtual std::string generateParseError(Client &client) const = 0;
		const std::string	&getType() const;
	};
}

#endif //ARESPONDER_HPP
