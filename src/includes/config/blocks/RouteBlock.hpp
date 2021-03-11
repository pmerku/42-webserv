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
		void parseData();

		std::string getType() const;

	private:
		std::string					_location;
		std::vector<std::string>	_allowedMethods;
		std::string					_root;
		bool						_directoryListing;
		std::string					_index;
		std::string					_cgi;
		std::string					_saveUploads;
		std::string					_proxyUrl;

	public:
		const std::string &getLocation() const;
		const std::vector<std::string> &getAllowedMethods() const;
		const std::string &getRoot() const;
		bool isDirectoryListing() const;
		const std::string &getIndex() const;
		const std::string &getCgi() const;
		const std::string &getSaveUploads() const;
		const std::string &getProxyUrl() const;
		/// return if it should serve files (if false -> proxy)
		bool 				shouldDoFile() const;

	};

}

#endif //ROUTEBLOCK_HPP