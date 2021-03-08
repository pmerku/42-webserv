//
// Created by jelle on 3/8/2021.
//

#ifndef REQUIREDKEY_HPP
#define REQUIREDKEY_HPP

#include "config/AConfigValidator.hpp"

namespace config {

	class RequiredKey: public AConfigBlockValidator {
	private:
		const std::string	_key;

	public:
		RequiredKey(const std::string &key);

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class RequiredKeyException: public std::exception {
		public:
			const char * what() const throw() {
				return "RequiredKey: key is not defined in block";
			}
		};
	};

}

#endif //REQUIREDKEY_HPP
