//
// Created by jelle on 3/10/2021.
//

#ifndef BOOLEANVALIDATOR_HPP
#define BOOLEANVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class BooleanValidator: public AConfigValidator {
	private:
		int 	_argnum;

	public:
		BooleanValidator(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class BooleanValidatorException: public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "Key {KEY} must be either 'true' or 'false'";
			}

		public:
			BooleanValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //BOOLEANVALIDATOR_HPP
