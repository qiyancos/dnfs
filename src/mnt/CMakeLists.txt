add_library(mnt
        mnt_null.cpp
        mnt_init.cpp
        mnt_mnt.cpp)
target_link_libraries(mnt utils)