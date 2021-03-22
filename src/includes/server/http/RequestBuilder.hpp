//
// Created by pmerku on 22/03/2021.
//

#ifndef REQUESTBUILDER_HPP
#define REQUESTBUILDER_HPP

#include <string>
#include <map>

namespace NotApache {

	class RequestBuilder {
	private:
		std::string 							_request;

		std::string								_method;
		std::string								_uri;
		std::string								_protocol;
		std::map<std::string, std::string>		_headerMap;
		std::string 							_body;

		static std::string endLine();

	public:
		RequestBuilder();
		explicit RequestBuilder(const std::string &method);

		RequestBuilder		&setURI(const std::string &path);
		RequestBuilder		&setHeader(const std::string &key, const std::string &value);
		RequestBuilder		&setBody(const std::string &data, size_t length);
		const std::string	&build();

		class RequestBuilderException : public std::exception {
		public:
			virtual const char *what() const throw() {
				return "Failed to build request";
			}
		};

		class MethodError : public RequestBuilderException {
		public:
			const char *what() const throw() {
				return "Unhandled method";
			}
		};

		class URIError : public RequestBuilderException {
		public:
			const char *what() const throw() {
				return "Missing / in front of URI";
			}
		};
	};

} // namespace NotApache

#endif //REQUESTBUILDER_HPP
