cmake_minimum_required(VERSION 3.10)
project(dnfs C)

set(CMAKE_C_STANDARD 11)
include_directories(include/utils)
include_directories(include/dnfsd)

add_executable(dnfsd
        tools/dnfsd/dnfsd.c
)