cmake_minimum_required(VERSION 3.10)
project(dnfs C CXX)
set(CMAKE_C_STANDARD 11)
add_subdirectory(tools/dnfsd
                 tools/mountd
        )