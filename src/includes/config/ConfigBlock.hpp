//
// Created by jelle on 3/8/2021.
//

#ifndef CONFIGBLOCK_HPP
#define CONFIGBLOCK_HPP

#include "config/ConfigLine.hpp"
#include <vector>
#include <exception>

namespace config {

	class ConfigBlock {
	protected:
		std::vector<ConfigLine>		_lines;
		std::vector<ConfigBlock*>	_blocks;
		ConfigBlock					*_parent;

	public:
		ConfigBlock(const ConfigLine &line, ConfigBlock *parent = 0);
		virtual ~ConfigBlock();

		static void validateEndBlock(const ConfigLine &line);

		void	addLine(const ConfigLine &line);
		void	addBlock(ConfigBlock *block);
		ConfigBlock	*getParent() const;
		void 	print(unsigned int spaces = 0) const;

		void	runPostValidators() const;

		// exceptions
		class ArgsWithBlockException: public std::exception {
		public:
			const char * what() const throw() {
				return "ConfigBlock: There may not be any arguments for block declarations";
			}
		};

		class BlockMissingOpeningException: public std::exception {
		public:
			const char * what() const throw() {
				return "ConfigBlock: Missing opening bracket for block declaration";
			}
		};
	};

}

#endif //CONFIGBLOCK_HPP
