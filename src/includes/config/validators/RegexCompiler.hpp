//
// Created by jelle on 3/30/2021.
//

#ifndef REGEXCOMPILER_HPP
#define REGEXCOMPILER_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class RegexCompiler : public AConfigValidator {
	private:
		int _argnum;

	public:
		RegexCompiler(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class RegexCompilerException : public ConfigException {
		protected:
			std::string getTemplate() const throw() {
				return "key {KEY} is not a valid regex pattern";
			}

		public:
			RegexCompilerException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //REGEXCOMPILER_HPP
