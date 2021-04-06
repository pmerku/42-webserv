//
// Created by jelle on 3/22/2021.
//

#ifndef HTTPMETHODVALIDATOR_HPP
#define HTTPMETHODVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class HTTPMethodValidator : public AConfigValidator {
	public:
		HTTPMethodValidator();

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class HTTPMethodValidatorException : public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "Invalid HTTP method as argument";
			}

		public:
			HTTPMethodValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //ARGUMENTLENGTH_HPP
