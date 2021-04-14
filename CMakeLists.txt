# cmake minimum version and project name
cmake_minimum_required(VERSION 3.16)
project(42-webserv)

# set compiler flags and standard
set(CMAKE_CXX_STANDARD 98)
set(FLAGS "-O3 -Wall -Wextra -Werror -pedantic-errors")
set(CMAKE_LINK_F)

# set cmake output directory
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/build)

if (APPLE)
	set(CMAKE_CXX_FLAGS "${FLAGS} -DBUILD_APPLE=1")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS "${FLAGS} -DBUILD_LINUX=1")
endif ()

# library files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/**.cpp
	${CMAKE_SOURCE_DIR}/src/**.c)

file(GLOB_RECURSE HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/**.hpp
	${CMAKE_SOURCE_DIR}/src/**.h)

# set the include directory
include_directories(src/_includes)
include_directories(src)

# create executable and link the library
add_executable(not-apache ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(not-apache pthread)
