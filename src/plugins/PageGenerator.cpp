//
// Created by pmerku on 14/04/2021.
//

#include "plugins/PageGenerator.hpp"

using namespace plugin;

PageGenerator::PageGenerator() : Plugin("page_404_gen") { }

PageGenerator::~PageGenerator() { }

bool PageGenerator::onHandleError(NotApache::HTTPClient &client, int code) {
	if (code == 404) {
		NotApache::globalLogger.logItem(logger::DEBUG, "Page Generator running");
		client.data.response.setResponse(
			client.data.response.builder.setStatus(code).setBody("Random text").build()
		);
		return true;
	}
	return false;
}
