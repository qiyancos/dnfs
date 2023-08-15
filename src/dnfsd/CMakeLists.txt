add_executable(dnfsd
        dnfsd.cpp
        dnfs_init.cpp
)

target_link_libraries(dnfsd
    PRIVATE
        log
        ntirpc
        utils
)