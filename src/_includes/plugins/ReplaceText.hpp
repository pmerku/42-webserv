//
// Created by pmerku on 15/04/2021.
//

#ifndef REPLACETEXT_HPP
#define REPLACETEXT_HPP

#include "plugins/Plugin.hpp"

namespace plugin {

	class ReplaceText : public Plugin {
	private:
		static const std::string _stringToReplace;
		static const std::string _stringToUse;

	public:
		ReplaceText();
		virtual ~ReplaceText();

		virtual bool onSendFile(NotApache::HTTPClient &client);
	};

} // namespace plugin

#endif //REPLACETEXT_HPP
