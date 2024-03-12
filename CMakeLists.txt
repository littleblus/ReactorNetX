cmake_minimum_required(VERSION 3.20)
project(my_muduo)

add_executable(server src/main.cpp)
add_executable(client6 src/test/testCli6.cpp)

target_compile_features(server PUBLIC cxx_std_20)
target_link_libraries(server pthread)
target_compile_features(client6 PUBLIC cxx_std_20)
target_link_libraries(client6 pthread)
