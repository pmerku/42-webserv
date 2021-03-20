//
// Created by pmerku on 3/20/2021.
//

#ifndef LOCALTIME_HPP
#define LOCALTIME_HPP

#include <string>

namespace utils {

	inline struct tm *relativeLocalTime(const time_t *t, struct tm *tm) {
		(void)t;
		return tm;
	}

} // namespace utils

#endif //LOCALTIME_HPP
