find_package(yaml-cpp REQUIRED)

include_directories(${YAML_INCLUDE_DIRS})

add_executable(dnfsd
        dnfsd.cpp
        dnfs_init.cpp
        dnfs_ntirpc.cpp
        dnfs_config.cpp
        dnfs_signal_proc.cpp
        dnfsd_exit.cpp
        dnfs_auth.cpp dnfs_exports.cpp)

target_link_libraries(dnfsd
    PRIVATE
        log
        ntirpc
        utils
        ${YAML_CPP_LIBRARIES}
        stdc++fs
)