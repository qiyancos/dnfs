add_library(nfs
        nfs_init.cpp
        nfs_xdr.cpp
        nfs_utils.cpp
        nfs_null.cpp
        nfs_getattr.cpp
        nfs_link.cpp
        nfs_readdirplus.cpp
        nfs_fsstat.cpp
        nfs_fsinfo.cpp
        nfs_pathconf.cpp)
target_link_libraries(nfs utils)