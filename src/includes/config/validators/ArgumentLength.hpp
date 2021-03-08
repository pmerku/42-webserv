//
// Created by jelle on 3/8/2021.
//

#ifndef ARGUMENTLENGTH_HPP
#define ARGUMENTLENGTH_HPP

#include "config/AConfigValidator.hpp"

namespace config {

	class ArgumentLength: public AConfigValidator {
	private:
		int	_min;
		int	_max;

	public:
		ArgumentLength(int exact);
		ArgumentLength(int min, int max);

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class ArgumentLengthException: public std::exception {
		public:
			const char * what() const throw() {
				return "Argument length: incorrect argument length";
			}
		};
	};

}

#endif //ARGUMENTLENGTH_HPP
