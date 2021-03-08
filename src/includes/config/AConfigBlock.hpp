//
// Created by jelle on 3/8/2021.
//

#ifndef ACONFIGBLOCK_HPP
#define ACONFIGBLOCK_HPP

#include "config/ConfigLine.hpp"
#include "config/AConfigValidator.hpp"
#include <vector>
#include <exception>
#include <map>

namespace config {

	class AConfigBlock {
	public:
		typedef	std::vector<const AConfigValidator *>		validatorListType;
		typedef std::map<std::string, validatorListType>	validatorsMapType;

	protected:
		std::vector<ConfigLine>		_lines;
		std::vector<AConfigBlock*>	_blocks;
		AConfigBlock				*_parent;

		virtual const validatorsMapType	&getValidators() const = 0;
		virtual const validatorListType	&getBlockValidators() const = 0;
		virtual const std::string		*getAllowedBlocks() const = 0;
		const validatorListType			&getValidatorForKey(const ConfigLine &line) const;
		void 							runValidatorForKey(const ConfigLine &line) const;

	public:
		AConfigBlock(const ConfigLine &line, AConfigBlock *parent = 0);
		virtual ~AConfigBlock();

		static void validateEndBlock(const ConfigLine &line);
		virtual const std::string	getType() const = 0;
		void 	print(unsigned int spaces = 0) const;

		void	addLine(const ConfigLine &line);
		void	addBlock(AConfigBlock *block);
		bool 	hasKey(const std::string &key) const;
		AConfigBlock	*getParent() const;

		void	runPostValidators() const;

		// exceptions
		class ArgsWithBlockException: public std::exception {
		public:
			const char * what() const throw() {
				return "AConfigBlock: There may not be any arguments for block declarations";
			}
		};

		class BlockMissingOpeningException: public std::exception {
		public:
			const char * what() const throw() {
				return "AConfigBlock: Missing opening bracket for block declaration";
			}
		};

		class UnknownKeyException: public std::exception {
		public:
			const char * what() const throw() {
				return "AConfigBlock: Unknown key encountered";
			}
		};

		class InvalidNestedBlockException: public std::exception {
		public:
			const char * what() const throw() {
				return "AConfigBlock: Cannot have that block inside of this block";
			}
		};
	};

}

#endif //ACONFIGBLOCK_HPP
