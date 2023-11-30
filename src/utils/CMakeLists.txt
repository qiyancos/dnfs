add_library(utils
        thread_util.cpp
        common_utils.cpp
        time_utils.cpp
        util_exception.cpp
        net_utils.cpp
        smart_ptr.cpp
        )

target_link_libraries(utils dl)