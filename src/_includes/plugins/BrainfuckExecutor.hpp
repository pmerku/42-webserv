//
// Created by Jelle on 14/04/2021.
//

#ifndef BRAINFUCKEXECUTOR_HPP
#define BRAINFUCKEXECUTOR_HPP

#include "plugins/Plugin.hpp"

namespace plugin {

	class BrainfuckExecutor : public Plugin {
	private:
	public:
		BrainfuckExecutor();
		virtual ~BrainfuckExecutor();

		virtual bool onSendFile(NotApache::HTTPClient &client);
	};

} // namespace plugin

#endif //BRAINFUCKEXECUTOR_HPP
