//
// Created by pmerku on 11/03/2021.
//

#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include <cstring>
#include <cerrno>

#include <ctime>
#include <sys/time.h> // have to use C header for gettimeofday()

#include <map>
#include <string>

namespace NotApache {

	template<typename T, typename U>
	class CreateMap {
	private:
		std::map<T, U> _map;

	public:
		CreateMap(const T &key, const U &value) {
			_map[key] = value;
		}

		CreateMap<T, U> &operator()(const T &key, const U &value) {
			_map[key] = value;
			return *this;
		}

		operator std::map<T, U>() {
			return _map;
		}
	};

	class ResponseBuilder {
	private:
		std::string								_response;

		std::string								_protocol;
		std::pair<std::string, std::string>		_statusLine;
		std::map<std::string, std::string>		_headerMap;
		std::string								_body;

		static const std::map<int, std::string>	_statusMap;

		static std::string	endLine();
		static std::string	convertTime(time_t currentTime);

	public:
		ResponseBuilder();
		explicit ResponseBuilder(const std::string &protocol);

		ResponseBuilder		&setStatus(int code);
		ResponseBuilder		&setHeader(const std::string &key, const std::string &value);
		ResponseBuilder		&setBody(const std::string &data, size_t length);
		ResponseBuilder		&setDate();
		const std::string	&build();

		class ResponseBuilderException : public std::exception {
		public:
			virtual const char *what() const throw() {
				return "Failed to build response";
			}
		};

		class DateError : public ResponseBuilderException {
		public:
			const char *what() const throw() {
				return "Failed to populate Date header";
			}
		};

		class StatusCodeError : public ResponseBuilderException {
		public:
			const char *what() const throw() {
				return "Unhandled status code";
			}
		};
	};

} // namespace NotApache

#endif //RESPONSEBUILDER_HPP
