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

#include "log/log.h"
#include "utils/common_utils.h"
#include "dnfsd/dnfs_config.h"

using namespace std;

#define MODULE_NAME "config"

// 服务启动配置信息
nfs_start_info_t nfs_start_info = {
    .dump_default_config = false
//    .lw_mark_trigger = false,
//    .drop_caps = true
};

/* DNFS的核心参数，与配置有关 */
nfs_parameter_t nfs_param;

/* 用于打印当前系统使用的配置信息 */
void dump_config() {
    /*TODO*/
}

/* 该函数用于快速从配置文件中获取指定层级的数据，
 * 并将结果按照指定格式写入到变量中，返回成功标志 */
template<typename T>
int config_get(T& out, const YAML::Node& config,
               const vector<string>& key_list) {
    int key_size = key_list.size();
    YAML::Node next_node = const_cast<YAML::Node&>(config);
    for (int i = 0; i < key_size; i++) {
        const string& key = key_list[i];
        YAML::NodeType::value type = config[key].Type();
        switch (type) {
            case YAML::NodeType::Map:
                if (i == key_size - 1) {
                    if (!(is_same<T, map<string, string>>::value or
                            is_same<T, map<string, char>>::value or
                            is_same<T, map<string, int>>::value or
                            is_same<T, map<string, long long>>::value or
                            is_same<T, map<string, float>>::value or
                            is_same<T, map<string, long double>>::value)) {
                        LOG(MODULE_NAME, L_WARN,
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
                    if (!(is_same<T, vector<string>>::value or
                            is_same<T, vector<char>>::value or
                            is_same<T, vector<int>>::value or
                            is_same<T, vector<long long>>::value or
                            is_same<T, vector<float>>::value or
                            is_same<T, vector<long double>>::value)) {
                        LOG(MODULE_NAME, L_WARN,
                            "Config file node for %s cannot be"
                            " interpreted as normal vector class",
                            format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node[key].as<T>();
                        return 0;
                    }
                } else {
                    LOG(MODULE_NAME, L_WARN,
                        "List type config node can not be "
                        "indexed by \"%s(%d)\" in %s)",
                        key.c_str(), i, format(key_list).c_str());
                    return 1;
                }
            case YAML::NodeType::Scalar:
                if (i == key_size - 1) {
                    if (!(is_same<T, string>::value or
                          is_same<T, char>::value or
                          is_same<T, int>::value or
                          is_same<T, long long>::value or
                          is_same<T, float>::value or
                          is_same<T, long double>::value)) {
                        LOG(MODULE_NAME, L_WARN,
                            "Config file node for %s cannot be"
                            " interpreted as normal value type",
                            format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node[key].as<T>();
                        return 0;
                    }
                } else {
                    LOG(MODULE_NAME, L_WARN,
                        "Value type config node can not be "
                        "indexed by \"%s(%d)\" in %s)",
                        key.c_str(), i, format(key_list).c_str());
                    return 1;
                }
            case YAML::NodeType::Null:
                LOG(MODULE_NAME, L_WARN,
                    "Unknown config node %s(%d) in %s",
                    key.c_str(), i, format(key_list).c_str());
                return 1;
            default:
                LOG(MODULE_NAME, L_WARN,
                    "Unknown config node type %d @%s(%d) in %s",
                    type, key.c_str(), i, format(key_list).c_str());
                return 1;
        }
        next_node = next_node[key];
    }
    /* Should never reach here */
    return 1;
}