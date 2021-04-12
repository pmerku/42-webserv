//
// Created by jelle on 3/11/2021.
//

#ifndef ATHREAD_HPP
#define ATHREAD_HPP

#include <queue>
#include "utils/mutex.hpp"
#include "server/communication/ServerEventBus.hpp"

namespace utils {

	class AThread {
	private:
		bool 			_isRunning;
		pthread_t 		_thread;
		virtual void	run() = 0;
		static void		*runPre(void *c);

	public:
		AThread();
		virtual ~AThread();

		void	start();
		void 	join() const;
	};


	template<class T>
	class AThreadQueue: public AThread {
	protected:
		MutexLock		            _queueMut;
		Mutex<bool>		            _isQueueRunning;
		bool 			            _shouldStop;
		std::queue<T *>	            _queue;
	    NotApache::ServerEventBus   _events;


		virtual void 	runQueue(T *task) = 0;
		virtual void 	disposeTask(T *task) {
			(void)task;
		};

		void run() {
			while (true) {
				_queueMut.lock();

				if (_shouldStop) {
					_queueMut.unlock();
					return;
				}

				if (_queue.size() == 0) {
					_isQueueRunning.set(false);
					_queueMut.unlock();
					_events.getPostedEvent(); // block until new item is added to the queue, doesnt matter what event it is
					_isQueueRunning.set(true);
					continue;
				}

				if (!_queue.empty()) {
					T *task = _queue.front();
					_queue.pop();
					_queueMut.unlock();
					runQueue(task);
				}
			}
		}

	public:
		AThreadQueue(): AThread(), _isQueueRunning(false), _shouldStop(false) {
			start();
		}

		virtual ~AThreadQueue() {
			_shouldStop = true;
			addTask(0);
			join();
			_queueMut.lock();
		}

		void 			addTask(T *task) {
			_queueMut.lock();
			if (task)
				_queue.push(task);

			// start queue if not running
			_isQueueRunning.lock();
			if (!*_isQueueRunning)
                _events.postEvent(NotApache::ServerEventBus::START_QUEUE); // tell queue to start again
			_isQueueRunning.unlock();

			_queueMut.unlock();
		}
	};
}

#endif //ATHREAD_HPP
