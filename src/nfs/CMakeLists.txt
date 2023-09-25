add_library(nfs
        nfs_null.cpp
        nfs_xdr.cpp
        nfs_utils.cpp
        nfs_fsinfo.cpp
        nfs_init.cpp)
target_link_libraries(nfs utils)