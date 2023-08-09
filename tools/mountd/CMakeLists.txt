cmake_minimum_required(VERSION 3.10)

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