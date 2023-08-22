find_package(yaml-cpp REQUIRED)

include_directories(${YAML_INCLUDE_DIRS})

add_executable(dnfsd
        dnfsd.cpp
        dnfs_init.cpp
        dnfs_ntirpc.cpp dnfs_config.cpp)

target_link_libraries(dnfsd
    PRIVATE
        log
        ntirpc
        utils
        ${YAML_CPP_LIBRARIES}
)