//
// Created by pmerku on 10/03/2021.
//

#ifndef ERRORTHROW_HPP
#define ERRORTHROW_HPP

#include <iostream>

#define ERROR_INFO __FILE__ << ":" << __LINE__ << " "

#ifdef DEBUG_THROW
# define ERROR_THROW_NO_ARG() do { \
	std::cerr << ERROR_INFO << std::endl; \
	throw; \
} while(0)

# define ERROR_THROW(x) do { \
	std::cerr << ERROR_INFO << std::endl; \
	throw x; \
} while(0)
#else
# define ERROR_THROW_NO_ARG() (throw)
# define ERROR_THROW(x) (throw x)
#endif

#endif //ERRORTHROW_HPP
