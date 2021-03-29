//
// Created by jelle on 3/26/2021.
//

#ifndef ISFILE_HPP
#define ISFILE_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class IsFile: public AConfigValidator {
	private:
		int	_arg;

	public:
		IsFile(int arg);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class IsFileException: public ConfigException {
		protected:
			const char *getTemplate() const throw() {
				return "key '{KEY}' in block '{BLOCK_NAME}' must be a path to a file";
			}

		public:
			IsFileException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};
	};

}

#endif //ISFILE_HPP
