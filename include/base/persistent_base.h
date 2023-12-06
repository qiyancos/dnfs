/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Piyuyang pi_yuyang@163.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#ifndef DNFSD_PERSISTENT_BASE_H
#define DNFSD_PERSISTENT_BASE_H

#include <string>
#include <map>
#include <iostream>
#include <exception>

#include "base/exception.h"
#include "base/serializable_base.h"

/*持久化数据*/
class PersistentBase
{

public:
    /*持久化
     * params path:持久化到的文件
     * */
    virtual void persist(const std::string &persistence_path) = 0;

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    virtual void resolve(const std::string &resolve_path) = 0;

    /*map存入文件
     * params: m_map 待存入的map
     * params: persistence_path 存入的文件路径
     * return: bool 是否成功
     * */
    template <typename KEY, typename VALUE>
    bool dump(const std::map<KEY, VALUE> &m_map, const std::string &persistence_path);

    /*从文件读取map
     * params: resolve_path 文件路径
     * params: m_map 读出的map
     * return: bool 是否成功
     * */
    template <typename KEY, typename VALUE>
    bool load(const std::string &resolve_path, std::map<KEY, VALUE> *m_map);
};

/*map存入文件
 * params: m_map 待存入的map
 * params: persistence_path 存入的文件路径
 * return: bool 是否成功
 * */
template <typename KEY, typename VALUE>
bool PersistentBase::dump(const std::map<KEY, VALUE> &m_map, const std::string &persistence_path)
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
        throw Exception("class %s does not support serialization", typeid(KEY).name());
        return false;
    }
    if (std::is_base_of<Serializable, VALUE>::value == false)
    {
        throw Exception("%s does not support serialization", typeid(VALUE).name());
        return false;
    }

    // 遍历m_map，放入数组
    int pair_size = sizeof(std::pair<KEY, VALUE>);
    std::pair<KEY, VALUE> *buffer = (std::pair<KEY, VALUE> *)malloc(pair_size * m_size);
    std::pair<KEY, VALUE> *ptr = buffer;
    for (const auto iter : m_map)
    {
        *ptr = std::pair<KEY, VALUE>{iter.first, iter.second};
        ptr += 1;
    }

    // 写文件
    FILE *f = fopen(persistence_path.c_str(), "w");
    size_t res = fwrite(&m_size, sizeof(int), 1, f);
    res = fwrite(buffer, pair_size, m_size, f);
    fclose(f);
    free(buffer);

    if (res != m_size)
    {
        std::cout << "write failed" << std::endl;
        return false;
    }

    return true;
}

/*从文件读取map
 * params: resolve_path 文件路径
 * params: m_map 读出的map
 * return: bool 是否成功
 * */
template <typename KEY, typename VALUE>
bool PersistentBase::load(const std::string &resolve_path, std::map<KEY, VALUE> *m_map)
{

    FILE *f = fopen(resolve_path.c_str(), "r");
    if (f == NULL)
    {
        return false;
    }
    // 读取map大小
    int m_size;
    fread(&m_size, sizeof(int), 1, f);

    // 读取pair数组
    int pair_size = sizeof(std::pair<KEY, VALUE>);
    std::pair<KEY, VALUE> *buffer = (std::pair<KEY, VALUE> *)malloc(pair_size * m_size);
    std::pair<KEY, VALUE> *ptr = buffer;
    size_t res = fread(buffer, pair_size, m_size, f);
    fclose(f);

    if (res != m_size)
    {
        std::cout << "read failed" << std::endl;
        free(buffer);
        return false;
    }

    // 放入map
    for (int i = 0; i < m_size; i++)
    {
        std::pair<KEY, VALUE> p = *(std::pair<KEY, VALUE> *)(ptr);
        ptr += 1;
        m_map->emplace(p.first, p.second);
    }
    free(buffer);

    return true;
}

#endif // DNFSD_PERSISTENT_BASE_H
