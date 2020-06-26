cmake_minimum_required(VERSION 3.14)
project(unit_testing CXX Fortran)

include(CTest)

add_subdirectory(extern)
add_subdirectory(test)

