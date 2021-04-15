//
// Created by Jelle on 14/04/2021.
//

#include "plugins/BrainfuckExecutor.hpp"
#include "server/http/HTTPResponder.hpp"

using namespace plugin;

BrainfuckExecutor::BrainfuckExecutor() : Plugin("brainfuck_executor") { }

BrainfuckExecutor::~BrainfuckExecutor() { }

bool BrainfuckExecutor::onSendFile(NotApache::HTTPClient &client) {
	// execute brainfuck code on .bf files
	if (client.file.getExt() == "bf") {
		try {
			NotApache::HTTPResponder::runBrainfuck(client);
		} catch (std::exception &e) {
			NotApache::globalLogger.logItem(logger::ERROR, std::string("Brainfuck error: ") + e.what());
			NotApache::HTTPResponder::handleError(client, 500);
			return true;
		}
		return true;
	}
	return false;
}
