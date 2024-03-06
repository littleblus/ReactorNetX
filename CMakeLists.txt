cmake_minimum_required(VERSION 3.20)
project(timewheel)

add_executable(server src/tcpSrv.cpp)
add_executable(client src/tcpCli.cpp)

target_compile_features(server PUBLIC cxx_std_17)
target_compile_features(client PUBLIC cxx_std_17)
