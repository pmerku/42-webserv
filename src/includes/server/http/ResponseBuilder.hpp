//
// Created by pmerku on 11/03/2021.
//

#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include <ctime>
#include <sys/time.h> // have to use C header for gettimeofday()
#include "utils/DataList.hpp"

#include <map>
#include <string>

namespace NotApache {

	class ResponseBuilder {
	private:
		std::string								_protocol;
		std::pair<std::string, std::string>		_statusLine;
		std::map<std::string, std::string>		_headerMap;
		utils::DataList							_body;

		static const std::string				_endLine;
		static std::string	convertTime(time_t currentTime);

		ResponseBuilder		&setDefaults();

	public:
		static const std::map<int, std::string>	statusMap;

		ResponseBuilder();
		explicit ResponseBuilder(const std::string &protocol);

		ResponseBuilder		&setStatus(int code);
		ResponseBuilder		&setHeader(const std::string &key, const std::string &value);
		ResponseBuilder		&setBody(const std::string &data, size_t length);
		ResponseBuilder		&setBody(const std::string &data);
		ResponseBuilder		&setBody(const utils::DataList &data);
		ResponseBuilder		&setServer();
		ResponseBuilder		&setConnection();
		ResponseBuilder		&removeHeader(const std::string &header);
		ResponseBuilder		&setDate();
		utils::DataList		build();
	};

} // namespace NotApache

#endif //RESPONSEBUILDER_HPP
