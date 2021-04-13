//
// Created by jelle on 3/8/2021.
//

#ifndef UNIQUE_HPP
#define UNIQUE_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class Unique: public AConfigValidator {
	public:
		Unique();

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class UniqueException: public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "You can only have one '{KEY}' in '{BLOCK_NAME}' block";
			}

		public:
			UniqueException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //UNIQUE_HPP
