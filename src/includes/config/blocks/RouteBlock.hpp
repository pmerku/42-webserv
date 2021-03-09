//
// Created by jelle on 3/8/2021.
//

#ifndef ROUTEBLOCK_HPP
#define ROUTEBLOCK_HPP

#include "config/AConfigBlock.hpp"

namespace config {

	class RouteBlock: public AConfigBlock {
	protected:
		static const validatorsMapType	_validators;
		static const validatorListType	_blockValidators;
		static const std::string		_allowedBlocks[];

		const validatorsMapType	&getValidators() const;
		const validatorListType	&getBlockValidators() const;
		const std::string		*getAllowedBlocks() const;

	public:
		RouteBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent = 0);
		static void	cleanup();

		const std::string 		getType() const;
	};

}

#endif //ROUTEBLOCK_HPP
