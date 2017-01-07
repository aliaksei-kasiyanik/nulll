cmake_minimum_required(VERSION 3.6)
project(nulll)

include_directories(linux-4.9.1/include)

set(SOURCE_FILES main.c)
add_executable(nulll ${SOURCE_FILES})