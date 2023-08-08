cmake_minimum_required(VERSION 3.10)
project(dnfs C CXX)
set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/utils)
add_executable(mounted
        auth.c
        cache.c
        fsloc.c
        mount_dispatch.c
        mountd.c
        rmtab.c
        svc_run.c
        v4root.c
)