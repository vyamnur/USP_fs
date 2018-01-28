cmake_minimum_required(VERSION 3.6)
project(hellofs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES hello.c hello_funs.c)
add_executable(hellofs ${SOURCE_FILES})