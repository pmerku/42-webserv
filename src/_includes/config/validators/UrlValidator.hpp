//
// Created by jelle on 3/26/2021.
//

#ifndef URLVALIDATOR_HPP
#define URLVALIDATOR_HPP

#include "config/AConfigValidator.hpp"
#include "config/ConfigException.hpp"

namespace config {

	class UrlValidator: public AConfigValidator {
	public:
		struct urlParsed {
			std::string	ip;
			int 		port;
			std::string	protocol;
		};

	private:
		int	_argnum;

	public:
		UrlValidator(int argnum);

		void test(const ConfigLine &line, const AConfigBlock &block) const;
		static urlParsed	parseUrl(const std::string &str);

		class UrlValidatorProtocolException: public ConfigException {
		protected:
			std::string	getTemplate() const throw() {
				return "url of '{KEY}' in block '{BLOCK_NAME}' has an unsupported protocol";
			}

		public:
			UrlValidatorProtocolException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};

		class UrlValidatorInvalidException: public ConfigException {
		protected:
			std::string	getTemplate() const throw() {
				return "url of '{KEY}' in block '{BLOCK_NAME}' is invalid. url must be ip and may not contain a path!";
			}

		public:
			UrlValidatorInvalidException(const ConfigLine &line, const AConfigBlock *block) : ConfigException(line, block) {};
		};

		class UrlParseProtocolError: public std::exception {
		public:
			const char * what() const throw() {
				return "Url protocol not supported";
			}
		};

		class UrlProtocolMissingError: public std::exception {
		  public:
			const char * what() const throw() {
				return "Url protocol is missing";
			}
		};

		class UrlParseError: public std::exception {
		public:
			const char * what() const throw() {
				return "Url could not be parsed";
			}
		};
	};

}

#endif //URLVALIDATOR_HPP
