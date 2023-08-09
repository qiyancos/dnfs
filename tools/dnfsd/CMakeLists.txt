cmake_minimum_required(VERSION 3.10)

add_executable(dnfsd
        dnfsd.c
        dnfssvc.c
        lockd.c
        dnfsinit.c
        dnfscache.c
)