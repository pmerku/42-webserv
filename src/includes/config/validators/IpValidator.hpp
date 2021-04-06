//
// Created by jelle on 3/26/2021.
//

#ifndef IPVALIDATOR_HPP
#define IPVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class IpValidator: public AConfigValidator {
	private:
		int	_arg;

	public:
		IpValidator(int arg);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class IpValidatorException: public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "key '{KEY}' in block '{BLOCK_NAME}' must be a valid ipv4 address";
			}

		public:
			IpValidatorException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //IPVALIDATOR_HPP
