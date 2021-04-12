//
// Created by jelle on 3/9/2021.
//

#ifndef MUTUALLYEXCLUSIVE_HPP
#define MUTUALLYEXCLUSIVE_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"
#include <vector>
#include <string>

namespace config {

	class MutuallyExclusive : public AConfigValidator {
	private:
		std::vector<std::string> _list;

	public:
		MutuallyExclusive(const std::string& str);

		void test(const ConfigLine &line, const AConfigBlock &block) const;

		class MutuallyExclusiveException : public ConfigException {
        private:
            const std::vector<std::string> &_list;

		protected:
			std::string getTemplate() const throw() {
			    std::string out = "Key {KEY} in block {BLOCK_NAME} is mutually exclusive with another key. Only one of the following may exist in this block: ";
			    for (std::vector<std::string>::const_iterator it = _list.begin(); it != _list.end();) {
				    out += *it;
				    ++it;
				    if (it != _list.end())
					    out += ", ";
			    }
				return out;
			}

		public:
			MutuallyExclusiveException(const ConfigLine &line, const AConfigBlock *block, const std::vector<std::string> &list): ConfigException(line, block), _list(list) {};
		};

		class MutuallyExclusiveMissingException : public ConfigException {
          private:
            const std::vector<std::string> &_list;

          protected:
			std::string getTemplate() const throw() {
                std::string out = "Key {KEY} in block {BLOCK_NAME} is mutually exclusive with another key. One of the following must exist: ";
                for (std::vector<std::string>::const_iterator it = _list.begin(); it != _list.end();) {
                    out += *it;
				    ++it;
                    if (it != _list.end())
                        out += ", ";
                }
                return out;
			}

		public:
			MutuallyExclusiveMissingException(const ConfigLine &line, const AConfigBlock *block, const std::vector<std::string> &list): ConfigException(line, block), _list(list) {};
		};
	};

}

#endif //MUTUALLYEXCLUSIVE_HPP
