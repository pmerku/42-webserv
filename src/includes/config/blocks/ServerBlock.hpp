//
// Created by jelle on 3/8/2021.
//

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "config/AConfigBlock.hpp"

namespace config {

	class ServerBlock: public AConfigBlock {
	protected:
		static const validatorsMapType	_validators;
		static const validatorListType	_blockValidators;
		static const std::string		_allowedBlocks[];

		const validatorsMapType	&getValidators() const;
		const validatorListType	&getBlockValidators() const;
		const std::string		*getAllowedBlocks() const;

	public:
		ServerBlock(const ConfigLine &line, AConfigBlock *parent = 0);
		virtual ~ServerBlock();

		const std::string 		getType() const;
	};

}

#endif //SERVERBLOCK_HPP
