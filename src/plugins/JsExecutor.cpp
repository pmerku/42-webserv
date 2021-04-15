//
// Created by Jelle on 14/04/2021.
//

#include "plugins/JsExecutor.hpp"
#include "server/http/HTTPResponder.hpp"

using namespace plugin;

JsExecutor::JsExecutor() : Plugin("js_executor") { }

JsExecutor::~JsExecutor() { }

bool JsExecutor::onSendFile(NotApache::HTTPClient &client) {
	// execute js code on .jsexec files
	if (client.file.getExt() == "jsexec") {
		try {
			NotApache::HTTPResponder::runJs(client);
		} catch (std::exception &e) {
			NotApache::globalLogger.logItem(logger::ERROR, std::string("JSExec error: ") + e.what());
			NotApache::HTTPResponder::handleError(client, 500);
			return true;
		}
		return true;
	}
	return false;
}

