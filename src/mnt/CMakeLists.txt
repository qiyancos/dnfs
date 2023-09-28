add_library(mnt
        mnt_null.cpp
        mnt_init.cpp
        mnt_mnt.cpp
        mnt_xdr.cpp
        mnt_umnt.cpp
        mnt_dump.cpp
        mnt_umntAll.cpp
        mnt_export.cpp)
target_link_libraries(mnt utils)