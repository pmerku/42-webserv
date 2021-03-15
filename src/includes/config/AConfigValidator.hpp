//
// Created by jelle on 3/8/2021.
//

#ifndef ACONFIGVALIDATOR_HPP
#define ACONFIGVALIDATOR_HPP

#include "config/ConfigLine.hpp"
#include "utils/ErrorThrow.hpp"

namespace config {

	class AConfigBlock;

	class AConfigValidator {
	protected:
		bool		_postValidator;

	public:
		AConfigValidator(bool isPostValidator = false);
		virtual ~AConfigValidator();
		bool			isPostValidator() const;
		virtual void	test(const ConfigLine &line, const AConfigBlock &block) const = 0;
	};

	class AConfigBlockValidator: public AConfigValidator {
	public:
		AConfigBlockValidator();
		virtual void	test(const ConfigLine &line, const AConfigBlock &block) const = 0;
	};
}

#endif // ACONFIGVALIDATOR_HPP
