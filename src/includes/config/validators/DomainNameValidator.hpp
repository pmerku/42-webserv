//
// Created by jelle on 3/30/2021.
//

#ifndef DOMAINNAMEVALIDATOR_HPP
#define DOMAINNAMEVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class DomainNameValidator : public AConfigValidator {
	private:
		int _argnum;

	public:
		DomainNameValidator(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class DomainNameValidatorException : public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "key {KEY} is not a valid domain name";
			}

		public:
			DomainNameValidatorException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //DOMAINNAMEVALIDATOR_HPP
