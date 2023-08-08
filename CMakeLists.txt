cmake_minimum_required(VERSION 3.10)
project(dnfs C CXX)
set(CMAKE_C_STANDARD 11)

execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("Get linux kernel version: ${KERNEL_VERSION}")

set(KERNEL_INCLUDE_DIR "/usr/src/linux-headers-${KERNEL_VERSION}/include")
message("Add linux kernel include dir: ${KERNEL_INCLUDE_DIR}")

include_directories(${KERNEL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(tools/dnfsd)