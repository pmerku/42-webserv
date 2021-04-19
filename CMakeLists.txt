# cmake minimum version and project name
cmake_minimum_required(VERSION 3.16)
project(42-webserv C CXX)

# set compiler flags and standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)
set(FLAGS "-O3 -Wall -Wextra -Werror -pedantic-errors")

# set debug flags on debug build
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG_THROW=1")

# set cmake output directory
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/build)

if (APPLE)
	set(CMAKE_C_FLAGS "${FLAGS} -DBUILD_APPLE=1")
	set(CMAKE_CXX_FLAGS "${FLAGS} -DBUILD_APPLE=1")
elseif(UNIX)
	set(CMAKE_C_FLAGS "${FLAGS} -DBUILD_LINUX=1")
	set(CMAKE_CXX_FLAGS "${FLAGS} -DBUILD_LINUX=1")
endif ()

# library files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/**.c
	${CMAKE_SOURCE_DIR}/src/**.cpp)

file(GLOB_RECURSE HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/**.h
	${CMAKE_SOURCE_DIR}/src/**.hpp)

# set the include directory
include_directories(src/_includes)
include_directories(src)

# create executable and link the library
add_executable(not-apache ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(not-apache pthread)
