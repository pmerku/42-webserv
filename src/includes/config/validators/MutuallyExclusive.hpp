//
// Created by jelle on 3/9/2021.
//

#ifndef MUTUALLYEXCLUSIVE_HPP
#define MUTUALLYEXCLUSIVE_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class MutuallyExclusive : public AConfigValidator {
	private:
		const std::string _one;
		const std::string _two;

	public:
		MutuallyExclusive(const std::string& one, const std::string& two);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class MutuallyExclusiveException : public ConfigException {
		protected:
			const char *getTemplate() const throw() {
				return "Key {KEY} in block {BLOCK_NAME} is mutually exclusive with another key. Only one of them must exist";
			}

		public:
			MutuallyExclusiveException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};

		class MutuallyExclusiveMissingException : public ConfigException {
		protected:
			const char *getTemplate() const throw() {
				return "Key {KEY} in block {BLOCK_NAME} is mutually exclusive with another key. One of them is missing";
			}

		public:
			MutuallyExclusiveMissingException(const ConfigLine &line, const AConfigBlock *block): ConfigException(line, block) {};
		};
	};

}

#endif //MUTUALLYEXCLUSIVE_HPP
