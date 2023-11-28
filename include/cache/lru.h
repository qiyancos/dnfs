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
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include <iostream>
#include <list>
#include <unordered_map>


#ifndef DNFSD_LRU_H
#define DNFSD_LRU_H

/*LRU*/
template<typename VALUE>
class LRU {
    typedef typename std::unordered_map<VALUE, typename std::list<VALUE>::iterator>::iterator m_it;
    struct LRUAddRes {
        /*是否推出最后元素 默认否*/
        bool is_pop = false;
        VALUE pop_item;
    };

private:
    /*缓存列表*/
    std::list<VALUE> m_list;
    /*哈希表，存储缓存列表节点对应的迭代器，协助查找数据*/
    std::unordered_map<VALUE, typename std::list<VALUE>::iterator> m_map;
    /*容量*/
    int capacity;

public:
    /*初始化*/
    LRU(int cap);

    /*添加*/
    LRUAddRes add(VALUE value) {
        LRUAddRes res;
        /*容量到达上限，删除最后一个元素*/
        if (m_list.size() == this->capacity) {
            VALUE end = m_list.back();
            res.is_pop = true;
            res.pop_item = end;
            m_list.pop_back();
            m_map.erase(end);
        }
        /*列表头部插入该元素*/
        m_list.emplace_front(value);
        /*更新哈希表*/
        m_map[value] = m_list.begin();
        return res;
    }

    /*查询*/
    void access(VALUE value);

    /*删除*/
    void remove(VALUE value);

    // /*打印当前缓存*/
    // void print()
    // {
    //     for (typename std::list<VALUE>::iterator i = m_list.begin(); i != m_list.end(); i++)
    //     {
    //         printf("%s,", *i);
    //     }
    //     std::cout << std::endl;
    // }
};

/*初始化*/
template<typename VALUE>
LRU<VALUE>::LRU(int cap) {
    capacity = cap;
}

/*查询*/
template<typename VALUE>
void LRU<VALUE>::access(VALUE value) {
    m_it iter = m_map.find(value);
    if (iter != m_map.end()) {
        /*元素移动到列表头部*/
        m_list.splice(m_list.begin(), m_list, iter->second);
        *iter->second = value;
    }
}

/*删除*/
template<typename VALUE>
void LRU<VALUE>::remove(VALUE value) {
    m_it iter = m_map.find(value);
    if (iter != m_map.end()) {
        /*列表删除该元素*/
        m_list.erase(iter->second);
        /*哈希表删除该元素*/
        m_map.erase(value);
    }
}

#endif //DNFSD_LRU_H
