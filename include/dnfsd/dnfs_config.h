/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#ifndef DNFSD_DNFS_CONFIG_H
#define DNFSD_DNFS_CONFIG_H

#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "log/log.h"
#include "utils/common_utils.h"
#include "dnfsd/dnfs_meta_data.h"

/* DNFS的启动配置信息，存放了相关的数据信息 */
typedef struct __nfs_start_info {
    int dump_default_config;
//    int lw_mark_trigger;
//    bool drop_caps;
} nfs_start_info_t;

/* DNFSD默认配置文件的结构体 */
extern YAML::Node dnfs_config;

// 服务启动配置信息
extern nfs_start_info_t nfs_start_info;

/* DNFS的核心参数，与配置有关 */
extern nfs_parameter_t nfs_param;

/*  */

/* 初始化配置文件 */
void init_config(const std::string& config_file_path);

/* 用于打印当前系统使用的配置信息 */
void dump_config();

/* 该函数用于快速从配置文件中获取指定层级的数据，并将结果按照指定格式写入到变量中，返回成功标志 */
/* 该函数用于快速从配置文件中获取指定层级的数据，
 * 并将结果按照指定格式写入到变量中，返回成功标志 */
template<typename T>
int config_get(T& out, const YAML::Node& config,
               const std::vector<std::string>& key_list) {
    int key_size = key_list.size();
    YAML::Node next_node(config);
    for (int i = 0; i < key_size; i++) {
        const std::string& key = key_list[i];
        YAML::NodeType::value type;
        try {
            type = next_node[key].Type();
        } catch (YAML::InvalidNode) {
            LOG("config", L_WARN,
                "Unknown config node \"%s\" in %s(@%d)",
                key.c_str(), format(key_list).c_str(), i);
            return 1;
        }
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
                        out = next_node[key].as<T>();
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
                        out = next_node[key].as<T>();
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
                        out = next_node[key].as<T>();
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
        next_node = YAML::Node(next_node[key]);
    }
    /* Should never reach here */
    return 1;
}

#endif //DNFSD_DNFS_CONFIG_H
