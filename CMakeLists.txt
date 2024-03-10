cmake_minimum_required(VERSION 3.20)
project(my_muduo)

add_executable(test src/main.cpp)

target_compile_features(test PUBLIC cxx_std_20)
target_link_libraries(test pthread)
