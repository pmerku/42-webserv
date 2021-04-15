//
// Created by Jelle on 14/04/2021.
//

#ifndef JSEXECUTOR_HPP
#define JSEXECUTOR_HPP

#include "plugins/Plugin.hpp"

namespace plugin {

	class JsExecutor : public Plugin {
	private:
	public:
		JsExecutor();
		virtual ~JsExecutor();

		virtual bool onSendFile(NotApache::HTTPClient &client);
	};

} // namespace plugin

#endif //JSEXECUTOR_HPP
