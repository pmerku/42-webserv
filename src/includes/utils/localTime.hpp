//
// Created by pmerku on 3/20/2021.
//

#ifndef LOCALTIME_HPP
#define LOCALTIME_HPP

#include <climits>

namespace utils {

//	inline int secondsToTm(long long t, struct tm *tm) {
//		long long days, seconds;
//		int remdays, remseconds, remyears;
//		int qc_cycles, c_cycles, q_cycles;
//		int years, months;
//		int wday, yday, leap;
//		static const char daysInMonth[] = { 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29 };
//
//		tm->
//	}
//
//	inline struct tm *relativeLocalTime(const time_t *t, struct tm *tm) {
//		if (*t < INT_MIN * 31622400LL || *t > INT_MAX * 31622400LL) {
//			errno = EOVERFLOW;
//			return NULL;
//		}
//		if (secondsToTm((long long)*t, tm) < 0) {
//			errno = EOVERFLOW;
//			return NULL;
//		}
//		return tm;
//	}

} // namespace utils

#endif //LOCALTIME_HPP
