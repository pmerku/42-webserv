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
	};

} // namespace plugin

#endif //JSTONSTATAPI_HPP
