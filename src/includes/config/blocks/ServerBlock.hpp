//
// Created by jelle on 3/8/2021.
//

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "config/AConfigBlock.hpp"
#include "config/blocks/RouteBlock.hpp"
#include <map>

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
		ServerBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent = 0);
		static void	cleanup();

		std::string getType() const;
		void 				parseData();

	private:
		int							_port;
		int							_bodyLimit;
		std::string					_host;
		std::string					_serverName;
		std::vector<RouteBlock*>	_routeBlocks;
		std::map<int ,std::string>	_errorPages;

	public:
		int								getPort() const;
		int								getBodyLimit() const;
		const std::string				&getHost() const;
		const std::vector<RouteBlock*>	&getRouteBlocks() const;
		const std::string				&getServerName() const;
		std::string						getErrorPage(int code) const;
		RouteBlock						*findRoute(const std::string &path);
	};

}

#endif //SERVERBLOCK_HPP
