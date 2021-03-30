//
// Created by jelle on 3/30/2021.
//

#ifndef ERRORCODEVALIDATOR_HPP
#define ERRORCODEVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"
#include <vector>

namespace config {

	class ErrorCodeValidator : public AConfigValidator {
	private:
		int 							_argnum;
		static const std::vector<int>	_errorCodes;

	public:
		ErrorCodeValidator(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class ErrorCodeValidatorException: public ConfigException {
		protected:
			const char *getTemplate() const throw() {
				return "Status code in {KEY} is not a valid error code";
			}

		public:
			ErrorCodeValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //ERRORCODEVALIDATOR_HPP
