cmake_minimum_required(VERSION 3.20)
project(ReactorNetX)

add_executable(server src/main.cpp)

target_compile_features(server PUBLIC cxx_std_20)
target_link_libraries(server pthread)
