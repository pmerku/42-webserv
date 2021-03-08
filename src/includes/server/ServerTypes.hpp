//
// Created by jelle on 3/2/2021.
//

#ifndef SERVERTYPES_HPP
#define SERVERTYPES_HPP

namespace NotApache {
	typedef int 	FD;

	enum ClientStates {
		READING,
		WRITING,
		CLOSED,
	};

	enum ResponseStates {
		PARSE_ERROR,
		IS_RESPONDING,
		IS_WRITING,
		ERRORED,
		TIMED_OUT,
	};

	enum ClientTypes {
		TERMINAL,
		CONNECTION
	};
}

#endif //SERVERTYPES_HPP
