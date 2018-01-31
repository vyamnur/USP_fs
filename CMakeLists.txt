cmake_minimum_required(VERSION 3.6)
project(hellofs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -l -D_FILE_OFFSET_BITS=64")


set(SOURCE_FILES hello.c hello_funs.c)
add_executable(hellofs ${SOURCE_FILES})