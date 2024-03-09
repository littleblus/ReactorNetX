cmake_minimum_required(VERSION 3.20)
project(my_muduo)

add_executable(echoServer src/main.cpp)

target_compile_features(echoServer PUBLIC cxx_std_17)
target_link_libraries(echoServer pthread)
