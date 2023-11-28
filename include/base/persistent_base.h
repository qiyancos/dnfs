/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Pi_yuyang@163.com
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
#ifndef DNFSD_PERSISTENT_BASE_H
#define DNFSD_PERSISTENT_BASE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "base/serializable_base.h"
// #include "base/base_exception.h"

/*持久化数据*/
class PersistentBase
{
public:
    /*持久化
     * params path:持久化到的文件
     * */
    virtual void persist(const std::string &persisence_path) = 0;

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    virtual void resolve(const std::string &resolve_path) = 0;

    /*map存入文件
     * params: m_map 待存入的map
     * params: persisence_path 存入的文件路径
     * return: bool 是否成功
     * */
    template <typename KEY, typename VALUE>
    bool save_to_file(std::map<KEY, VALUE> m_map, const std::string &persisence_path)
    {
        // 得到m_map键值对数量
        int m_size = m_map.size();
        if (m_size == 0)
        {
            return false;
        }

        // 判断m_map中键、值是否可以序列化（是否为Serializable的子类）
        if (std::is_base_of<Serializable, KEY>::value == false)
        {
            std::cout << "does not support serialization" << std::endl;
            // throw BaseException("The key class '%s' does not support serialization", typeid(KEY).name());
            return false;
        }
        if (std::is_base_of<Serializable, VALUE>::value == false)
        {
            std::cout << "does not support serialization" << std::endl;
            // throw BaseException("The value class '%s' does not support serialization", typeid(VALUE).name());
            return false;
        }

        // 遍历m_map，调用序列化方法
        KEY *key_buffer = (KEY *)malloc(sizeof(KEY) * m_size);
        KEY *k_ptr = key_buffer;
        VALUE *value_buffer = (VALUE *)malloc(sizeof(VALUE) * m_size);
        VALUE *v_ptr = value_buffer;
        for (const auto iter : m_map)
        {
            *k_ptr = iter.first;
            k_ptr += 1;
            *v_ptr = iter.second;
            v_ptr += 1;
        }

        // 存入文件
        FILE *f = fopen(persisence_path.c_str(), "w");
        size_t res = fwrite(&m_size, sizeof(int), 1, f);
        std::cout << "write m_size res: " << res << std::endl;
        res = fwrite(key_buffer, sizeof(KEY), m_size, f);
        std::cout << "write key_buffer res: " << res << std::endl;
        res = fwrite(value_buffer, sizeof(VALUE), m_size, f);
        std::cout << "write value_buffer res: " << res << std::endl;
        fclose(f);

        free(key_buffer);
        free(value_buffer);
        return true;
    }

    /*从文件读取map
     * params: resolve_path 文件路径
     * params: m_map 读出的map
     * return: bool 是否成功
     * */
    template <typename KEY, typename VALUE>
    bool read_from_file(const std::string &resolve_path, std::map<KEY, VALUE> *m_map){
        KEY k;
        VALUE v;
        return true;
    }
};

#endif // DNFSD_PERSISTENT_BASE_H
