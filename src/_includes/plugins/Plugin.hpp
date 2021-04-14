//
// Created by pmerku on 14/04/2021.
//

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <string>
#include "server/global/GlobalLogger.hpp"
#include "server/http/HTTPClient.hpp"

namespace plugin {

	class Plugin {
	private:
		std::string	_id;

	public:
		explicit Plugin(const std::string &name);
		virtual ~Plugin();

		virtual bool onHandleError(NotApache::HTTPClient &client, int code);

		const std::string &getId() const;

		bool operator==(const std::string &name) const;
		bool operator==(const char *name) const;
	};

} // namespace plugin

#endif //PLUGIN_HPP
