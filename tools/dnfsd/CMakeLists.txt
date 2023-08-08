cmake_minimum_required(VERSION 3.10)
project(dnfs C CXX)
set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(dnfsd
        dnfsd.c
        dnfsinit.c
        dnfssvc.c
)