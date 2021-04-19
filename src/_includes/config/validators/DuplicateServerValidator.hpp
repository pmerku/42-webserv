//
// Created by jelle on 4/19/2021.
//

#ifndef DUPLICATESERVERVALIDATOR_HPP
#define DUPLICATESERVERVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class ServerBlock;

	class DuplicateServerValidator : public AConfigValidator {
	public:
		DuplicateServerValidator();

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class DuplicateServerValidatorException : public ConfigException {
		protected:
			std::string	getTemplate() const throw() {
				return "All server blocks must be a unique combination of Port, Host and servername";
			}

		public:
			DuplicateServerValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //ARGUMENTLENGTH_HPP
