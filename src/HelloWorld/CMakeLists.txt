cmake_minimum_required(VERSION 3.25)

project(HELLO_PROJECT LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Config types" FORCE)

add_executable(hello_project main.cpp)
target_compile_features(hello_project PRIVATE cxx_std_20)