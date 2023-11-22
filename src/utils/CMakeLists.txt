add_library(utils
        thread_util.cpp
        common_utils.cpp
        time_utils.cpp
        unit_exception.cpp
        net_utils.cpp
        serializable_base.cpp)

target_link_libraries(utils dl)