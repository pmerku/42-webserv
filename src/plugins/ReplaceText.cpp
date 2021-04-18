//
// Created by pmerku on 15/04/2021.
//

#include "plugins/ReplaceText.hpp"
#include "server/http/HTTPResponder.hpp"
#include <string>

using namespace plugin;

const std::string ReplaceText::_stringToReplace = "42";
const std::string ReplaceText::_stringToUse = "CODAM";

ReplaceText::ReplaceText() : Plugin("replace_text") { }

ReplaceText::~ReplaceText() { }

bool ReplaceText::onSendFile(NotApache::HTTPClient &client) {
	// replaces all spaces with 42
	if (true) {
		utils::DataList::DataListIterator it = client.data.response.getAssociatedDataRaw().beginList();
		utils::DataList::DataListIterator pos = it;
		utils::DataList newList;

		for (; it != client.data.response.getAssociatedDataRaw().endList(); it = client.data.response.getAssociatedDataRaw().find(_stringToReplace, it)) {
			newList.subList(newList, pos, it);
			newList.add(_stringToUse.c_str(), _stringToUse.length());

			std::advance(it, _stringToReplace.length());
			if (it == client.data.response.getAssociatedDataRaw().endList()) {
				break;
			}

			client.data.response.getAssociatedDataRaw().resize(it, client.data.response.getAssociatedDataRaw().endList());
			pos = client.data.response.getAssociatedDataRaw().beginList();
		}
		client.data.response.getAssociatedDataRaw().clear();
		client.data.response.getAssociatedDataRaw() = newList;
		return true;
	}
	return false;
}
