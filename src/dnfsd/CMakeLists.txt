add_executable(dnfs
        dnfsd_main.cpp
        dnfs_init.cpp
        dnfs_dispatcher.cpp
        dnfs_worker.cpp
        dnfs_admin.cpp
)

target_link_libraries(dnfs
    PRIVATE
        log
        ntirpc
)