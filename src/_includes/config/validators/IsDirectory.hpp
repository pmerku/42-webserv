//
// Created by jelle on 3/26/2021.
//

#ifndef ISDIRECTORY_HPP
#define ISDIRECTORY_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class IsDirectory: public AConfigValidator {
	private:
		int	_arg;

	public:
		IsDirectory(int arg);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class IsDirectoryException: public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "key '{KEY}' in block '{BLOCK_NAME}' must be a path to a directory";
			}

		public:
			IsDirectoryException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //ISDIRECTORY_HPP
