//
// Created by pmerku on 11/03/2021.
//

#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "AResponse.hpp"

#include <cstring>
#include <cerrno>

#include <ctime>
#include <sys/time.h> // have to use C header for gettimeofday()

namespace response {

	template<typename T, typename U>
	class createMap {
	private:
		std::map<T, U> _map;

	public:
		createMap(const T &key, const U &value) {
			_map[key] = value;
		}

		createMap<T, U> &operator()(const T &key, const U &value) {
			_map[key] = value;
			return *this;
		}

		operator std::map<T, U>() {
			return _map;
		}
	};

	class ResponseBuilder {
	private:
		std::string _response;

		static const std::map<int, std::string> _statusMap;
		ResponseBuilder		&setEndLine();
		ResponseBuilder		&setContentHeader(size_t length);
		ResponseBuilder		&convertTime(time_t currentTime);

	public:
		explicit ResponseBuilder(const std::string &protocol);

		ResponseBuilder		&setStatus(int code);
		ResponseBuilder		&setHeader(const std::string &key, const std::string &value);
		ResponseBuilder		&setBody(const std::string &data, size_t length);
		ResponseBuilder		&setDate();
		const std::string	&build() const;
	};

}

#endif //RESPONSEBUILDER_HPP
