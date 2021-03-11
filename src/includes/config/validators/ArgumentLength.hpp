//
// Created by jelle on 3/8/2021.
//

#ifndef ARGUMENTLENGTH_HPP
#define ARGUMENTLENGTH_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class ArgumentLength: public AConfigValidator {
	private:
		int	_min;
		int	_max;

	public:
		ArgumentLength(int exact);
		ArgumentLength(int min, int max);

		void	test(const ConfigLine &line, const AConfigBlock &block) const;

		class ArgumentLengthException: public ConfigException {
		protected:
			const char * getTemplate() const throw() {
				return "Argument length for {BLOCK_NAME}/{KEY} is invalid";
			}

		public:
			ArgumentLengthException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //ARGUMENTLENGTH_HPP
