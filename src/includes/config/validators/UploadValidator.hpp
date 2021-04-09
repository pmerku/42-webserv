//
// Created by jelle on 3/9/2021.
//

#ifndef UPLOADVALIDATOR_HPP
#define UPLOADVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class UploadValidator : public AConfigValidator {
	public:
		UploadValidator();

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class UploadValidatorException : public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "Key {KEY} in block {BLOCK_NAME} is missing, it is required for allowed_methods PUT and DELETE";
			}

		public:
			UploadValidatorException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //UPLOADVALIDATOR_HPP
