cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
project(lls)

add_subdirectory(src)
