cmake_minimum_required(VERSION 2.6)
project(lls)

add_executable(lls main.cpp)

install(TARGETS lls RUNTIME DESTINATION bin)
