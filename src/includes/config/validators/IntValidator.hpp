//
// Created by jelle on 3/9/2021.
//

#ifndef INTVALIDATOR_HPP
#define INTVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"
#include <climits>

namespace config {

	class IntValidator : public AConfigValidator {
	private:
		int	_min;
		int _max;
		int _argnum;

	public:
		IntValidator(int argnum, int min = INT_MIN, int max = INT_MAX);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class IntValidatorException : public ConfigException {
		protected:
			const char *getTemplate() const throw() {
				return "Key {KEY} must be a number";
			}

		public:
			IntValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};

		class IntValidatorRangeException : public ConfigException {
		protected:
			const char *getTemplate() const throw() {
				return "Key {KEY} is outside of range";
			}

		public:
			IntValidatorRangeException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //INTVALIDATOR_HPP
