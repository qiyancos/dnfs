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
#include "base/persistent_base.h"

#include <cstring>

class SerializableA final : public Serializable
{
public:
    int a;

    explicit SerializableA(int aa) { a = aa; };

    /*重载<*/
    bool operator<(const SerializableA &other_ptr) const
    {
        return a < other_ptr.a;
    }

    /*序列化*/
    void serialize() override{};

    /*反序列化*/
    void *deserialize() override { return nullptr; };
};

class SerializableB final : public Serializable
{
public:
    float b;

    explicit SerializableB(float bb) { b = bb; };

    /*重载<*/
    bool operator<(const SerializableB &other_ptr) const
    {
        return b < other_ptr.b;
    }

    /*序列化*/
    void serialize() override{};

    /*反序列化*/
    void *deserialize() override { return nullptr; };
};

class Persistent final : public PersistentBase
{
public:
    Persistent(){};
    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path) override{};

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    void resolve(const std::string &resolve_path) override{};
};

int main()
{
    std::map<SerializableA, SerializableB> m_map = {};
    m_map.emplace(SerializableA(1), SerializableB(1));
    m_map.emplace(SerializableA(2), SerializableB(2));
    Persistent p;
    p.dump<SerializableA, SerializableB>(m_map, "/workspaces/dnfs/cmake-build-debug/test/base/tmp.bin");
    std::cout << "-----------" << std::endl;

    std::map<SerializableA, SerializableB> new_map = {};
    p.load<SerializableA, SerializableB>("/workspaces/dnfs/cmake-build-debug/test/base/tmp.bin", &new_map);
    auto iter = new_map.end();
    iter--;
    std::cout << iter->first.a << "," << iter->second.b << std::endl;
    return 0;
}