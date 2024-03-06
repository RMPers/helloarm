cmake_minimum_required(VERSION 3.22)
project(esoa VERSION 0.1
             DESCRIPTION "Explore SIMD on Arm")
add_executable(main src/main.cpp)

SET( CMAKE_CXX_FLAGS  "-march=armv8-a+sve")
