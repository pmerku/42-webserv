//
// Created by jelle on 3/8/2021.
//

#ifndef UNIQUE_HPP
#define UNIQUE_HPP

#include "config/AConfigValidator.hpp"

namespace config {

	class Unique: public AConfigValidator {
	public:
		Unique();

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class UniqueException: public std::exception {
		public:
			const char * what() const throw() {
				return "Unique: key cannot be used twice in a block";
			}
		};
	};

}

#endif //UNIQUE_HPP
