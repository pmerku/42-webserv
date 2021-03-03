//
// Created by jelle on 3/3/2021.
//

#ifndef APARSER_HPP
#define APARSER_HPP

#include <vector>
#include "server/Client.hpp"

namespace NotApache {
	class AParser {
	protected:
		ClientTypes	_type;

	public:
		enum parseState {
			UNFINISHED,
			FINISHED,
			PARSE_ERROR,
		};

		explicit AParser(ClientTypes type = CONNECTION);

		virtual ClientTypes	getType() const;
		virtual void 		setType(ClientTypes type);

		virtual parseState	parse(Client &client) const = 0;

		static parseState	runParsers(std::vector<AParser *> &parsers, Client &client);
	};
}

#endif //APARSER_HPP
