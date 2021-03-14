//
// Created by jelle on 3/14/2021.
//

#include "server/handlers/ThreadHandler.hpp"

using namespace NotApache;

ThreadHandlerTask::ThreadHandlerTask(HTTPClient &c, ThreadHandlerTask::TaskType t): client(c), type(t) {}

ThreadHandler::ThreadHandler(): StandardHandler(), utils::AThreadQueue<ThreadHandlerTask>() {}

void ThreadHandler::runQueue(ThreadHandlerTask *task) {
	if (task->type == ThreadHandlerTask::WRITE)
		StandardHandler::write(task->client);
	else
		StandardHandler::read(task->client);
	disposeTask(task);
}

void ThreadHandler::read(HTTPClient &client) {
	addTask(new ThreadHandlerTask(client, ThreadHandlerTask::READ));
}

void ThreadHandler::write(HTTPClient &client) {
	addTask(new ThreadHandlerTask(client, ThreadHandlerTask::WRITE));
}

void ThreadHandler::disposeTask(ThreadHandlerTask *task) {
	delete task;
}
