//
// Created by pmerku on 14/04/2021.
//

#ifndef PAGEGENERATOR_HPP
#define PAGEGENERATOR_HPP

#include "plugins/Plugin.hpp"

namespace plugin {

	class PageGenerator : public Plugin {
	private:
		static const std::vector<std::string>	_facts;
		static const std::string				_template;

	public:
		PageGenerator();
		virtual ~PageGenerator();

		virtual bool onHandleError(NotApache::HTTPClient &client, int code);
	};

} // namespace plugin

#endif //PAGEGENERATOR_HPP
