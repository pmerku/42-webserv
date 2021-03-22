//
// Created by pmerku on 22/03/2021.
//

#ifndef CREATEVECTOR_HPP
#define CREATEVECTOR_HPP

#include <vector>

namespace utils {

	template<typename T>
	class CreateVector {
	private:
		std::vector<T> _vector;

	public:
		explicit CreateVector(const T &value) {
			_vector.push_back(value);
		}

		CreateVector<T> &operator()(const T &value) {
			_vector.push_back(value);
			return *this;
		}

		operator std::vector<T>() {
			return _vector;
		}
	};

} // namespace utils

#endif //CREATEVECTOR_HPP
