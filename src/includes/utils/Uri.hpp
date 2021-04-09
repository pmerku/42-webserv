//
// Created by jelle on 3/29/2021.
//

#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <exception>

namespace utils {

	class Uri {
	public:
		std::string path;
		std::string	query;
		std::string identifier;

		Uri();
		Uri(const std::string &path);

		void		appendPath(const std::string &path, bool fixDirectoryTraversal = false);

		void		setQuery();
		void		setQuery(const std::string &query);
		void		setIdentifier();
		void		setIdentifier(const std::string &id);
		void		setPath(const std::string &path);
		std::string	getExt() const;
		std::string	getFull() const;

		class InvalidPathException: public std::exception {
		public:
			const char * what() const throw() {
				return "Invalid path";
			}
		};
	};

}

#endif //URI_HPP
