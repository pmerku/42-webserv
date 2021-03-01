# cmake minimum version and project name
cmake_minimum_required(VERSION 3.16)
project(42-webserv)

# set compiler flags and standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -Werror")

# set cmake output directory
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/build)

# library files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/**.cpp)

file(GLOB_RECURSE HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/**.hpp)

# set the include directory
include_directories(include)
include_directories(src)

# create executable and link the library
add_executable(not-apache ${SOURCE_FILES} ${HEADER_FILES})

