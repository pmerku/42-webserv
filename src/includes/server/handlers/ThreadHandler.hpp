//
// Created by jelle on 3/11/2021.
//

#ifndef THREADHANDLER_HPP
#define THREADHANDLER_HPP

#include "utils/AThread.hpp"
#include "server/Client.hpp"
#include "server/handlers/StandardHandler.hpp"

namespace NotApache {

	class ThreadHandlerTask {
	public:
		enum TaskTypes {
			READ,
			WRITE
		};

		TaskTypes	taskType;
		Client		*client;

		ThreadHandlerTask(TaskTypes type, Client *c);
	};

	class ThreadHandler: public utils::AThreadQueue<ThreadHandlerTask>, public StandardHandler {
	private:
		void runQueue(ThreadHandlerTask *task);

		virtual void disposeTask(ThreadHandlerTask *task);

	public:
		virtual void read(Client &client);

		virtual void write(Client &client);

	public:
		ThreadHandler();
		virtual ~ThreadHandler();
	};

}

#endif //THREADHANDLER_HPP
