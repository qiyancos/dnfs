cmake_minimum_required(VERSION 3.10)
project(dnfsd CXX C)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")  #设置c++的编译选项
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")  #设置c的编译选项

message("Using c compiler: ${CMAKE_C_COMPILER}(${CMAKE_C_COMPILER_VERSION})")

# 获取系统内核版本和相应的内核include目录
execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("Get linux kernel version: ${KERNEL_VERSION}")

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

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/libntirpc/ntirpc)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        "${CMAKE_SOURCE_DIR}/src/libntirpc/cmake/modules/")

#[[set(BOOST_ROOT /usr/include)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
    MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
    MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
endif ()]]

add_subdirectory(${CMAKE_SOURCE_DIR}/src/libntirpc)
add_subdirectory(src/utils)
add_subdirectory(src/log)
add_subdirectory(src/dnfsd)

#[[测试日志]]
add_subdirectory(test/log)
#[[测试配置文件读取]]
add_subdirectory(test/dnfs_config)