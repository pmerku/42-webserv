//
// Created by pmerku on 22/03/2021.
//

#ifndef ENVBUILDER_HPP
#define ENVBUILDER_HPP

#include <map>
#include <string>

namespace CGIenv {

	class ENVBuilder {
	private:
		std::map<std::string, std::string>	_metaVariables;

	public:
		ENVBuilder();

		ENVBuilder	&AUTH_TYPE(const std::string &value);
		ENVBuilder	&CONTENT_LENGTH(const std::string &value);
		ENVBuilder	&CONTENT_TYPE(const std::string &value);
		ENVBuilder	&DOCUMENT_ROOT(const std::string &value);
		ENVBuilder	&GATEWAY_INTERFACE(const std::string &value);
		ENVBuilder	&PATH_INFO(const std::string &value);
		ENVBuilder	&PATH_TRANSLATED(const std::string &value);
		ENVBuilder	&QUERY_STRING(const std::string &value);
		ENVBuilder	&REMOTE_ADDR(const std::string &value);
		ENVBuilder	&REMOTE_IDENT(const std::string &value);
		ENVBuilder	&REMOTE_USER(const std::string &value);
		ENVBuilder	&REQUEST_METHOD(const std::string &value);
		ENVBuilder	&REQUEST_URI(const std::string &value);
		ENVBuilder	&SCRIPT_NAME(const std::string &value);
		ENVBuilder	&SERVER_NAME(const std::string &value);
		ENVBuilder	&SERVER_PORT(const std::string &value);
		ENVBuilder	&SERVER_PROTOCOL(const std::string &value);
		ENVBuilder	&SERVER_SOFTWARE(const std::string &value);
		ENVBuilder	&REDIRECT_STATUS(const std::string &value);
		ENVBuilder	&EXPORT(const std::string &key, const std::string &value);

		char		**build();
	};

} // namespace NotApache

#endif //ENVBUILDER_HPP
