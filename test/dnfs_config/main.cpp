#include <iostream>
#include <yaml-cpp/yaml.h>
#include "dnfsd/dnfs_config.h"

int main()
{
//    std::cout << "初始化yaml" << std::endl;
//    std::string config_file_path = "/workspaces/dnfs/test/dnfs_config/test.yaml";
//    YAML::Node config = YAML::LoadFile(config_file_path);
//
//    std::cout << "读取日志配置" << std::endl;
//    nfs_logging_config logging_config;
//    init_logging_config(logging_config, config);
//    std::cout << logging_config.backup_count << std::endl;

    std::shared_ptr<char> str;
    str.reset((char*)malloc(sizeof(char) * 128));
    std::cout << str.use_count() << std::endl;
    std::shared_ptr<char> str2 = str;
    std::cout << str.use_count() << std::endl;
}