cmake_minimum_required(VERSION 3.10)
project(dnfs C CXX)

message("Using c compiler: ${CMAKE_C_COMPILER}(${CMAKE_C_COMPILER_VERSION})")

set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin -fno-stack-protector")

#set(USR_INCLUDE_DIR "/usr/include")
#message("Add linux user-level include dir: ${USR_INCLUDE_DIR}")
#include_directories(${USR_INCLUDE_DIR})

# 获取系统内核版本和相应的内核include目录
execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("Get linux kernel version: ${KERNEL_VERSION}")

#set(KERNEL_INCLUDE_DIR "/usr/src/linux-headers-${KERNEL_VERSION}/include")
#message("Add linux kernel include dir: ${KERNEL_INCLUDE_DIR}")
#include_directories(${KERNEL_INCLUDE_DIR})

#set(KERNEL_UAPI_INCLUDE_DIR "/usr/src/linux-headers-${KERNEL_VERSION}/include/uapi")
#message("Add linux kernel uapi include dir: ${KERNEL_UAPI_INCLUDE_DIR}")
#include_directories(${KERNEL_UAPI_INCLUDE_DIR})

# 获取系统的架构已经架构关联的include目录
if (CMAKE_SYSTEM_PROCESSOR MATCHES "unknown")
    # uname -p is broken on this system.  Try uname -m
    EXECUTE_PROCESS( COMMAND uname -m
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            OUTPUT_VARIABLE SYSTEM_ARCH)
else (CMAKE_SYSTEM_PROCESSOR MATCHES "unknown")
    set(SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif (CMAKE_SYSTEM_PROCESSOR MATCHES "unknown")

if (SYSTEM_ARCH MATCHES "x86_64")
    set(INCLUDE_ARCH "x86")
elseif (SYSTEM_ARCH MATCHES "aarch64")
    set(INCLUDE_ARCH "arm64")
elseif (SYSTEM_ARCH MATCHES "arm64")
    set(INCLUDE_ARCH "arm64")
else()
    message(FATAL_ERROR "Unhandled architecture ${SYSTEM_ARCH}")
endif ()
set(ARCH ${INCLUDE_ARCH})
message("Get linux system arch: ${SYSTEM_ARCH}, include arch: ${INCLUDE_ARCH}")

#set(ARCH_INCLUDE_DIR "/usr/src/linux-headers-${KERNEL_VERSION}/arch/${INCLUDE_ARCH}/include")
#message("Add linux arch-based include dir: ${ARCH_INCLUDE_DIR}")
#include_directories(${ARCH_INCLUDE_DIR})

#set(ARCH_ASM_INCLUDE_DIR "/usr/src/linux-headers-${KERNEL_VERSION}/arch/${INCLUDE_ARCH}/include/generated/uapi")
#message("Add linux arch-based asm include dir: ${ARCH_ASM_INCLUDE_DIR}")
#include_directories(${ARCH_ASM_INCLUDE_DIR})

#set(ARCH_USR_INCLUDE_DIR "/usr/include/${SYSTEM_ARCH}-linux-gnu")
#message("Add linux user-level arch-based include dir: ${ARCH_USR_INCLUDE_DIR}")
#include_directories(${ARCH_USR_INCLUDE_DIR})

#set(GCC_INCLUDE_DIR "/usr/lib/gcc/${SYSTEM_ARCH}-linux-gnu/${CMAKE_C_COMPILER_VERSION}/include")
#message("Add cc compiler stdlib include dir: ${GCC_INCLUDE_DIR}")
#include_directories(${GCC_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories()

add_subdirectory(tools/dnfsd)
add_subdirectory(tools/mountd)
