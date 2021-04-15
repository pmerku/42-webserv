//
// Created by mramadan on 14/04/2021.
//

#ifndef JSTONSTATAPI_HPP
#define JSTONSTATAPI_HPP

#include "plugins/Plugin.hpp"

namespace plugin {

	class JsonStatAPI : public Plugin {
	private:
	public:
		JsonStatAPI();
		virtual ~JsonStatAPI();

		virtual bool onBeforeFileServing(NotApache::HTTPClient &client);

		class NotFound : public std::exception {
			public:
				virtual const char* what() const throw() { return "file not found"; }
		};
	};

} // namespace plugin

#endif //JSTONSTATAPI_HPP
