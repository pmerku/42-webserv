//
// Created by pmerku on 22/03/2021.
//

#ifndef CREATEMAP_HPP
#define CREATEMAP_HPP

#include <map>

namespace utils {

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

} // namespace utils

#endif //CREATEMAP_HPP
