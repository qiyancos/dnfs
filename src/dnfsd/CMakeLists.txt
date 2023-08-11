add_executable(dnfs
        dnfsd_main.c
        dnfs_init.c
        dnfs_dispatcher.c
        dnfs_worker.c
        dnfs_admin.c
)

target_link_libraries(dnfs
    PRIVATE
        log
        ntirpc
)