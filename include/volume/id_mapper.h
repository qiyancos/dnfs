/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#ifndef DNFSD_ID_MAPPER_H
#define DNFSD_ID_MAPPER_H
/*volume id管理器，id 自增，二分查找，当id达到最大值时查询空闲id进行重载*/
/*使用模板类进行操作*/
#include <cstdint>
#include <map>
#include <cstdarg>

template<typename T, typename... Args>
class IdMapper {
private:
    /*上一个人使用的id*/
    uint64_t use_id = 0;
    /*超出异常*/
    bool over_limit = false;
    /*存储 id-卷 列表*/
    std::map<uint64_t, T> id_valume_map;
public:
    /*添加数据
     * params ...:数据构造参数
     * return 数据的自增id
     * */
    uint32_t emplace_item(const Args &... rest);

    /*删除数据
     * params id:删除的id
     * */
    void delete_item(const uint64_t &id);

    /*查找数据
     * params id:查找的id
     * return 查找到的数据
     * */
    T* search_item(const uint64_t &id);

};


/*添加数据
 * params ...:添加构造参数
 * return 数据的自增id，0表示全部id使用完
 * */
template<typename T, typename... Args>
uint32_t IdMapper<T, Args...>::emplace_item(const Args &... rest) {
    /*如果没有超出限制*/
    if (!over_limit) {
        /*id自增*/
        use_id += 1;
        /*构建构建对象*/
        id_valume_map.emplace(use_id, rest...);
        /*判断超出标志*/
        over_limit = use_id == UINT64_MAX;
        return use_id;
    } else {
        /*todo，终止程序*/
        return 0;
    }
}

/*删除数据
 * params id:删除的id
 * */
template<typename T, typename... Args>
void IdMapper<T, Args...>::delete_item(const uint64_t &id) {
    id_valume_map.erase(id);
}

/*查找数据
 * params id:查找的id
 * return 查找到的数据
 * */
template<typename T, typename... Args>
T* IdMapper<T, Args...>::search_item(const uint64_t &id) {
    /*查询数据*/
    auto result=id_valume_map.find(id);
    /*判断数据是否存在*/
    if(result!=id_valume_map.end()){
        return &result->second;
    }
    return nullptr;
}

#endif //DNFSD_ID_MAPPER_H
