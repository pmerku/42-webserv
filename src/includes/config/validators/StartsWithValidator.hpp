//
// Created by jelle on 3/30/2021.
//

#ifndef STARTSWITHVALIDATOR_HPP
#define STARTSWITHVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class StartsWithValidator : public AConfigValidator {
	private:
		int 	_argnum;
		char _c;
		bool _reverse;

	public:
		StartsWithValidator(int argnum, char c, bool reverse = false);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class StartsWithValidatorException : public ConfigException {
		private:
			char c;

		protected:
			std::string getTemplate() const throw() {
				return std::string("key {KEY} must start with ") + c;
			}

		public:
			StartsWithValidatorException(const ConfigLine &line, const AConfigBlock *block, char ch): ConfigException(line, block), c(ch) {};
		};

		class StartsWithValidatorReverseException : public ConfigException {
		private:
			char c;

		protected:
			std::string getTemplate() const throw() {
				return std::string("key {KEY} cannot start with ") + c;
			}

		public:
			StartsWithValidatorReverseException(const ConfigLine &line, const AConfigBlock *block, char ch): ConfigException(line, block), c(ch) {};
		};
	};

}

#endif //STARTSWITHVALIDATOR_HPP
