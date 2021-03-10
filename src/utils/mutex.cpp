//
// Created by jelle on 3/10/2021.
//

#include "utils/mutex.hpp"

using namespace utils;

MutexLock::MutexLock(bool locked): _mut() {
	if (pthread_mutex_init(&_mut, 0) != 0)
		throw MutexCreateException();
	if (locked && pthread_mutex_lock(&_mut) != 0) {
		pthread_mutex_destroy(&_mut);
		throw MutexCreateException();
	}
}

MutexLock::~MutexLock() {
	pthread_mutex_destroy(&_mut);
}

void MutexLock::lock() {
	if (pthread_mutex_lock(&_mut) != 0)
		throw MutexLockException();
}

void MutexLock::unlock() {
	if (pthread_mutex_unlock(&_mut) != 0)
		throw MutexLockException();
}
