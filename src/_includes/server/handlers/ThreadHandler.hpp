//
// Created by jelle on 3/3/2021.
//

#ifndef THREADHANDLER_HPP
#define THREADHANDLER_HPP

#include "server/handlers/StandardHandler.hpp"
#include "utils/AThread.hpp"

namespace NotApache {

	class ThreadHandlerTask {
	public:
		enum TaskType {
			READ,
			WRITE
		};

		HTTPClient	&client;
		TaskType	type;

		ThreadHandlerTask(HTTPClient &client, TaskType type);
	};

	class ThreadHandler: public StandardHandler, public utils::AThreadQueue<ThreadHandlerTask> {
	public:
		ThreadHandler();

		void			runQueue(ThreadHandlerTask *task);

		virtual void	read(HTTPClient &client);
		virtual void	write(HTTPClient &client);

	protected:
		virtual void disposeTask(ThreadHandlerTask *task);

	};
}

#endif //THREADHANDLER_HPP
