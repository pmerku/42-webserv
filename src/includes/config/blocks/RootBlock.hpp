//
// Created by jelle on 3/8/2021.
//

#ifndef ROOTBLOCK_HPP
#define ROOTBLOCK_HPP

#include "config/AConfigBlock.hpp"
#include "config/blocks/ServerBlock.hpp"

namespace config {

	class RootBlock: public AConfigBlock {
	protected:
		static const validatorsMapType	_validators;
		static const validatorListType	_blockValidators;
		static const std::string		_allowedBlocks[];

		const validatorsMapType	&getValidators() const;
		const validatorListType	&getBlockValidators() const;
		const std::string		*getAllowedBlocks() const;

	public:
		RootBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent = 0);
		static void	cleanup();

		std::string getType() const;
		void 				parseData();


	private:
		int							_workerCount;
		std::vector<ServerBlock*>	_serverBlocks;

	public:
		int								getWorkerCount() const;
		const std::vector<ServerBlock*>	&getServerBlocks() const;
		ServerBlock						*findServerBlock(const std::string &host, int port = 80) const;

	};

}

#endif //ROOTBLOCK_HPP
