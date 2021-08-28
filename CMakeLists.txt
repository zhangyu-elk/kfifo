cmake_minimum_required(VERSION 3.19)
project(kfifo_test)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(kfifo_test kfifo.h kfifo_test.c)
