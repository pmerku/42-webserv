//
// Created by jelle on 3/8/2021.
//

#ifndef ACONFIGVALIDATOR_HPP
#define ACONFIGVALIDATOR_HPP

#include "config/ConfigLine.hpp"

namespace config {

	class AConfigValidator {
	protected:
		bool		_postValidator;
	public:
		AConfigValidator(bool isPostValidator = false);
		bool			isPostValidator() const;
		virtual bool	test(const ConfigLine &line, const std::string &block) const = 0;
	};

}

#endif // ACONFIGVALIDATOR_HPP
