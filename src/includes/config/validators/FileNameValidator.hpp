//
// Created by jelle on 3/30/2021.
//

#ifndef FILENAMEVALIDATOR_HPP
#define FILENAMEVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class FileNameValidator : public AConfigValidator {
	private:
		int _argnum;

	public:
		FileNameValidator(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class FileNameValidatorException : public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "key {KEY} is not a valid file name";
			}

		public:
			FileNameValidatorException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //FILENAMEVALIDATOR_HPP
