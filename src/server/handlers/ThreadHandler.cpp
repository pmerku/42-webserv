//
// Created by jelle on 3/11/2021.
//

#include "server/handlers/ThreadHandler.hpp"

using namespace NotApache;

ThreadHandlerTask::ThreadHandlerTask(ThreadHandlerTask::TaskTypes type, Client *c): taskType(type), client(c) {}

void ThreadHandler::runQueue(ThreadHandlerTask *task) {
	if (task->taskType == ThreadHandlerTask::READ)
		StandardHandler::read(*(task->client));
	else
		StandardHandler::write(*(task->client));
}

ThreadHandler::ThreadHandler(): AThreadQueue() {}
ThreadHandler::~ThreadHandler() {
	// dispose of tasks
	while (!_queue.empty()) {
		delete _queue.front();
		_queue.pop();
	}
	_queueMut.unlock();
	_queueWaitMut.unlock();
}

void ThreadHandler::read(Client &client) {
	addTask(new ThreadHandlerTask(ThreadHandlerTask::READ, &client));
}

void ThreadHandler::write(Client &client) {
	addTask(new ThreadHandlerTask(ThreadHandlerTask::WRITE, &client));
}

void ThreadHandler::disposeTask(ThreadHandlerTask *task) {
	delete task;
}
