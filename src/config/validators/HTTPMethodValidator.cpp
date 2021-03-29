//
// Created by jelle on 3/22/2021.
//

#include "config/validators/HTTPMethodValidator.hpp"
#include "server/http/RequestBuilder.hpp"
#include <algorithm>
#include "utils/ErrorThrow.hpp"

using namespace config;

HTTPMethodValidator::HTTPMethodValidator() : AConfigValidator(false) {}

void HTTPMethodValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	for (ConfigLine::arg_size i = 0; i < line.getArgLength(); ++i) {
		if (std::find(NotApache::RequestBuilder::methodArray.begin(), NotApache::RequestBuilder::methodArray.end(), line.getArg(i)) == NotApache::RequestBuilder::methodArray.end())
			ERROR_THROW(HTTPMethodValidatorException(line, &block));
	}
}
