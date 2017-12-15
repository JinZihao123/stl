cmake_minimum_required(VERSION 3.9)
project(STL)

set(CMAKE_CXX_STANDARD 11)

add_library(STL library.cpp library.h stl_alloc.h stl_iterator.h type_traits.h stl_construct.h)