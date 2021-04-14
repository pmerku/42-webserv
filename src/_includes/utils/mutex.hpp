//
// Created by jelle on 3/10/2021.
//

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <pthread.h>
#include <exception>

namespace utils {

	class MutexLock {
	private:
		pthread_mutex_t	_mut;

	public:
		MutexLock(bool locked = false);
		virtual ~MutexLock();

		void	lock();
		void 	unlock();

		class MutexCreateException: public std::exception {
			const char * what() const throw() {
				return "MutexLock: Failed to init mutex";
			}
		};

		class MutexLockException: public std::exception {
			const char * what() const throw() {
				return "MutexLock: Failed to lock/unlock mutex";
			}
		};
	};

	template<class T>
	class Mutex: public MutexLock {
	private:
		T	_value;

	public:
		Mutex(const T &initial): MutexLock(false), _value(initial) {}

		const T	&get() {
			return _value;
		}

		void	set(const T &newValue) {
			lock();
			setNoLock(newValue);
			unlock();
		};

		bool	setIf(const T &compare, const T &newValue) {
			lock();
			if (compare == get()) {
				setNoLock(newValue);
				unlock();
				return true;
			}
			unlock();
			return false;
		}

		void	setNoLock(const T &newValue) {
			_value = newValue;
		};

		Mutex	&operator=(const T &rhs) {
			set(rhs);
			return *this;
		}

		const T	&operator*() {
			return get();
		}
	};


}

#endif //MUTEX_HPP
