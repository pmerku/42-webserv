//
// Created by pmerku on 15/04/2021.
//

#include "plugins/ReplaceText.hpp"
#include "server/http/HTTPResponder.hpp"
#include <string>

using namespace plugin;

const std::string ReplaceText::_needle = "42";
const std::string ReplaceText::_newNeedle = "CODAM";

ReplaceText::ReplaceText() : Plugin("replace_text") { }

ReplaceText::~ReplaceText() { }

bool ReplaceText::onSendFile(NotApache::HTTPClient &client) {
	// replaces all needles with newNeedle
	utils::DataList::DataListIterator it = client.data.response.getAssociatedDataRaw().beginList();

	while (true) {
		it = client.data.response.getAssociatedDataRaw().findAndReplaceOne(_needle, _newNeedle, it);
		if (it == client.data.response.getAssociatedDataRaw().endList())
			break;
		std::advance(it, _newNeedle.length());
	}
	return false; // this plugin will pass on the modified text to the other plugins
}
