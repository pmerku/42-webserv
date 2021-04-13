//
// Created by jelle on 3/8/2021.
//

#ifndef CONFIGVALIDATORBUILDER_HPP
#define CONFIGVALIDATORBUILDER_HPP

#include "config/AConfigBlock.hpp"
#include "config/AConfigValidator.hpp"

namespace config {

	class ConfigValidatorListBuilder {
	private:
		AConfigBlock::validatorListType	_validators;

	public:
		ConfigValidatorListBuilder				&add(const AConfigValidator *validator);
		const AConfigBlock::validatorListType	&build() const;
	};

	class ConfigValidatorBuilder {
	private:
		AConfigBlock::validatorsMapType	_validators;

	public:
		ConfigValidatorBuilder					&addKey(const std::string &key, const AConfigBlock::validatorListType validators);
		const AConfigBlock::validatorsMapType	&build() const;
	};

}

#endif //CONFIGVALIDATORBUILDER_HPP
