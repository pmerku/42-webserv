//
// Created by jelle on 3/11/2021.
//

#ifndef ATHREAD_HPP
#define ATHREAD_HPP

namespace utils {

	class AThread {
	private:
		virtual void	run() = 0;

	public:
		AThread();
		virtual ~AThread();
	};

}

#endif //ATHREAD_HPP
