/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include <experimental/filesystem>

#include "dnfsd/dnfsd_exit.h"
#include "dnfsd/dnfs_config.h"

using namespace std;

#define MODULE_NAME "config"

/* DNFSD全局默认的YAML文件结构 */
YAML::Node dnfs_yaml_config;

/* DNFSD的全局配置参数属性结构体 */
dnfs_runtime_config dnfs_config;

// 服务启动配置信息
nfs_start_info_t nfs_start_info = {
    .dump_default_config = false
//    .lw_mark_trigger = false,
//    .drop_caps = true
};

/* DNFS的核心参数，与配置有关 */
nfs_parameter_t nfs_param;

/* 默认配置文件的字符表达形式 */
static const char default_config[] =
        "# default config file content\n"
        "log:\n"
        "    limit_type: time\n"
        "    limit_info: MIDNIGHT\n"
        "    backup_count: 30\n"
        "    formatter: \"%(levelname) <%(asctime)><PID-%(process)>: %(message)\"\n"
        "    path: /var/log/dnfsd/runtime.log\n";

/* 该函数用于快速从配置文件中获取指定层级的数据，并将结果按照指定格式写入到变量中，返回成功标志 */
/* 该函数用于快速从配置文件中获取指定层级的数据，
 * 并将结果按照指定格式写入到变量中，返回成功标志 */
template<typename T>
[[maybe_unused]] int config_get(T& out, const YAML::Node& config,
               const std::vector<std::string>& key_list) {
    int key_size = key_list.size();
    YAML::Node* next_node = new YAML::Node(config);
    for (int i = 0; i < key_size; i++) {
        const std::string& key = key_list[i];
        YAML::NodeType::value type;
        try {
            type = (*next_node)[key].Type();
        } catch (YAML::InvalidNode) {
            LOG("config", L_WARN,
                "Unknown config node \"%s\" in %s(@%d)",
                key.c_str(), format(key_list).c_str(), i);
            return 1;
        }
        YAML::Node* last_node = next_node;
        next_node = new YAML::Node((*next_node)[key]);
        delete last_node;
        switch (type) {
            case YAML::NodeType::Map:
                if (i == key_size - 1) {
                    if (!(std::is_same<T, std::map<std::string, std::string>>::value or
                          std::is_same<T, std::map<std::string, char>>::value or
                          std::is_same<T, std::map<std::string, int>>::value or
                          std::is_same<T, std::map<std::string, long long>>::value or
                          std::is_same<T, std::map<std::string, float>>::value or
                          std::is_same<T, std::map<std::string, long double>>::value)) {
                        LOG("config", L_WARN,
                            "Config file node for %s cannot be"
                            " interpreted as normal map class",
                            format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node->as<T>();
                        return 0;
                    }
                }
                break;
            case YAML::NodeType::Sequence:
                if (i == key_size - 1) {
                    if (!(std::is_same<T, std::vector<std::string>>::value or
                          std::is_same<T, std::vector<char>>::value or
                          std::is_same<T, std::vector<int>>::value or
                          std::is_same<T, std::vector<long long>>::value or
                          std::is_same<T, std::vector<float>>::value or
                          std::is_same<T, std::vector<long double>>::value)) {
                        LOG("config", L_WARN,
                            "Config file node for %s cannot be"
                            " interpreted as normal vector class",
                            format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node->as<T>();
                        return 0;
                    }
                } else {
                    LOG("config", L_WARN,
                        "List type config node can not be "
                        "indexed by \"%s\" in %s(@%d))",
                        key.c_str(), format(key_list).c_str(), i);
                    return 1;
                }
            case YAML::NodeType::Scalar:
                if (i == key_size - 1) {
                    if (!(std::is_same<T, std::string>::value or
                          std::is_same<T, char>::value or
                          std::is_same<T, int>::value or
                          std::is_same<T, long long>::value or
                          std::is_same<T, float>::value or
                          std::is_same<T, long double>::value)) {
                        LOG("config", L_WARN,
                            "Config file node for %s cannot be"
                            " interpreted as normal value type",
                            format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node->as<T>();
                        return 0;
                    }
                } else {
                    LOG("config", L_WARN,
                        "Value type config node can not be "
                        "indexed by \"%s\" in %s(@%d))",
                        key.c_str(), format(key_list).c_str(), i);
                    return 1;
                }
            default:
                LOG("config", L_WARN,
                    "Unknown config node type %d for \"%s\" in %s(@%d)",
                    type, key.c_str(), format(key_list).c_str(), i);
                return 1;
        }
    }
    /* Should never reach here */
    return 1;
}

/* 创建一个默认初始化的配置 */
static void init_default_config(const string& config_file_path) {
    string config_dir = config_file_path.substr(0,
                                                config_file_path.find_last_of('/'));
    try {
        experimental::filesystem::create_directories(config_dir);
    } catch (exception& e) {
        fprintf(stderr,
                "Failed to create directory \"%s\" for config file: %s.\n",
                config_dir.c_str(), e.what());
        exit_process(-1);
    }

    ofstream config_file_os;
    try {
        config_file_os.open(config_file_path.c_str(), ios::out);
    } catch (exception& e) {
        fprintf(stderr,
                "Failed to create default config file \"%s\"\n",
                config_file_path.c_str());
        exit_process(-1);
    }
    config_file_os << default_config;
    config_file_os.close();
}

/* 解析日志配置 */
int init_logging_config(dnfs_logging_config& out) {
    try {
        YAML::Node log_node = dnfs_yaml_config["log"];
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log node not found in config file: %s\n",
                e.what());
        return -1;
    }

    try {
        out.path = dnfs_yaml_config["log"]["path"].as<string>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.path missing\n");
    }

    try {
        out.limit_type = dnfs_yaml_config["log"]["limit_type"].as<string>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.limit_type missing\n");
    }

    try {
        out.limit_info = dnfs_yaml_config["log"]["limit_info"].as<string>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.limit_info missing\n");
    }

    try {
        out.backup_count = dnfs_yaml_config["log"]["backup_count"].as<int>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.backup_count missing\n");
    } catch (YAML::BadConversion e) {
        fprintf(stderr, "Error: log.backup_count \"%s\" format is not int\n",
                dnfs_yaml_config["log"]["backup_count"].as<string>().c_str());
        return -1;
    }

    try {
        out.formatter = dnfs_yaml_config["log"]["formatter"].as<string>();
    } catch (YAML::InvalidNode) {
        fprintf(stderr, "Warn: log.formatter missing\n");
    }
    return 0;
}

/* 初始化配置文件并进行解析 */
void init_config(const string& config_file_path) {
    if (access(config_file_path.c_str(), F_OK) == -1) {
        /* 如果目标文件不存在，创建一个默认初始化的配置 */
        fprintf(stdout, "Start init default config file\n");
        init_default_config(config_file_path);
    }

    try {
        dnfs_yaml_config = YAML::LoadFile(config_file_path);
    } catch (exception& e) {
        fprintf(stderr, "Failed to load config file %s: %s\n",
                config_file_path.c_str(), e.what());
        exit_process(-1);
    }

    /* 首先从配置文件中读取log相关的配置 */
    fprintf(stdout, "Loading config for logging from config file\n");
    if (init_logging_config(dnfs_config.log_config)) {
        fprintf(stderr, "Failed to load log config\n");
        exit_process(-1);
    }
}

/* 用于打印当前系统使用的配置信息 */
void dump_config() {
    /*TODO*/
}