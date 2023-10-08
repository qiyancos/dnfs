add_library(nfs
        nfs_null.cpp
        nfs_xdr.cpp
        nfs_utils.cpp
        nfs_fsinfo.cpp
        nfs_init.cpp
        nfs_pathconf.cpp
        nfs_getattr.cpp
        nfs_fsstat.cpp
        nfs_readdirplus.cpp)
target_link_libraries(nfs utils)