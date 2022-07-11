
cmake_minimum_required(VERSION 3.0.0)
project(ima VERSION 0.1.0)

add_library(libima INTERFACE)
target_include_directories(libima INTERFACE inc)
target_compile_features(libima INTERFACE cxx_std_11)
target_compile_options(libima INTERFACE "-Wall" "-Wextra" "-Wpedantic")

add_executable(ima main.cpp)
target_compile_features(ima PUBLIC cxx_std_11)
target_link_libraries(ima PUBLIC libima)