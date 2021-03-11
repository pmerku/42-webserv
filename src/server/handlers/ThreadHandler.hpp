//
// Created by jelle on 3/11/2021.
//

#ifndef THREADHANDLER_HPP
#define THREADHANDLER_HPP

#include "utils/AThread.hpp"

namespace NotApache {

	class ThreadHandler: public utils::AThread {
	private:
		void	run();
	};

}

#endif //THREADHANDLER_HPP
