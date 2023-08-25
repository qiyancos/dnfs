find_package(yaml-cpp REQUIRED)
include_directories(${YAML_INCLUDE_DIRS})
add_library(dnfs
    ../../src/dnfsd/dnfs_config.cpp
)
add_executable(dnfs_config_test main.cpp)
target_link_libraries(dnfs_config_test dnfs log ntirpc utils ${YAML_CPP_LIBRARIES} stdc++fs)