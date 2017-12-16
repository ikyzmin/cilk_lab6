cmake_minimum_required(VERSION 3.9)
project(cilk_lab6)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-fcilkplus -fopenmp -std=c++11")

add_executable(cilk_lab6 main.cpp)
target_compile_options(cilk_lab6 PUBLIC -fcilkplus)
set(CMAKE_VERBOSE_MAKEFILE ON)