//
// Created by jelle on 3/11/2021.
//

#include "utils/AThread.hpp"
#include <pthread.h>

using namespace utils;

AThread::AThread(): _isRunning(false), _thread() {}

AThread::~AThread() {}

void AThread::start() {
	if (_isRunning) return;
	::pthread_create(&_thread, 0, AThread::runPre, this);
}

void *AThread::runPre(void *c) {
	reinterpret_cast<AThread *>(c)->_isRunning = true;
	reinterpret_cast<AThread *>(c)->run();
	return 0;
}

void AThread::join() const {
	::pthread_join(_thread, 0);
}
