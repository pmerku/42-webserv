//
// Created by jelle on 3/9/2021.
//

#include "config/validators/UploadValidator.hpp"

using namespace config;

UploadValidator::UploadValidator() : AConfigValidator(true) {}

void UploadValidator::test(const ConfigLine &line, const AConfigBlock &block) const {
	(void)line;
	bool hasUploadMethod = false;
	if (block.hasKey("allowed_methods")) {
		const ConfigLine *allowedMethods = block.getKey("allowed_methods");
		for (ConfigLine::arg_size i = 0; i < allowedMethods->getArgLength(); ++i) {
			if (allowedMethods->getArg(i) == "PUT" || allowedMethods->getArg(i) == "DELETE") {
				hasUploadMethod = true;
				break;
			}
		}
	}
	if (hasUploadMethod && !block.hasKey("save_uploads"))
		ERROR_THROW(UploadValidatorException(ConfigLine("save_uploads", block.getLineNumber()), &block));
}
