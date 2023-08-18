cmake_minimum_required(VERSION 3.10)
project(log_use)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
add_subdirectory(src/log)
add_executable(log_use main.cpp)
target_link_libraries(log_use log)
