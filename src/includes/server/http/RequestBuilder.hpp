//
// Created by pmerku on 22/03/2021.
//

#ifndef REQUESTBUILDER_HPP
#define REQUESTBUILDER_HPP

#include <string>
#include <map>
#include <vector>
#include "utils/DataList.hpp"
#include "server/http/HTTPParseData.hpp"
#include <ctime>
#include <sys/time.h> // have to use C header for gettimeofday()

namespace NotApache {

	class RequestBuilder {
	private:
		std::string								_method;
		std::string								_uri;
		std::string								_protocol;
		std::map<std::string, std::string>		_headerMap;
		utils::DataList							_body;

		static const std::string				_endLine;

		static std::string	convertTime(time_t currentTime);
		RequestBuilder		&setDefaults();

	public:
		static const std::vector<std::string>	methodArray;

		RequestBuilder();
		explicit RequestBuilder(const std::string &method);
		explicit RequestBuilder(const HTTPParseData &data);

		RequestBuilder		&setURI(const std::string &path);
		RequestBuilder		&setHeader(const std::string &key, const std::string &value);
		RequestBuilder		&setBody(const std::string &data, size_t length);
		RequestBuilder		&setBody(const std::string &data);
		RequestBuilder		&setBody(const utils::DataList &data);
		RequestBuilder		&removeHeader(const std::string &header);
		RequestBuilder		&setDate();
		RequestBuilder		&setProtocol(const std::string &protocol);
		RequestBuilder		&setProtocol();
		utils::DataList		build();
	};

} // namespace NotApache

#endif //REQUESTBUILDER_HPP
