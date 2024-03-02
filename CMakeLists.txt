cmake_minimum_required(VERSION 3.27)
project(timewheel)

add_executable(time_wheel timewheel.cpp)

target_compile_features(time_wheel PUBLIC cxx_std_17)
