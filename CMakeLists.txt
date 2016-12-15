cmake_minimum_required(VERSION 2.8)

project(thread_pool)

set(CMAKE_BUILD_TYPE Release)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -D_DEBUG")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-unused-parameter")
	set(LINK_LIBS ${LINK_LIBS} pthread)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP -D_WIN32_WINNT=0x0601")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin)

include_directories(ext ext/catch include)

add_executable(thread_pool src/main.cpp)
target_link_libraries(thread_pool ${LINK_LIBS})

enable_testing()
add_test(thread_pool ${PROJECT_BINARY_DIR}/../bin/thread_pool)
