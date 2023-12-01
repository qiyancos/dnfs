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

#include <cstring>
#include <typeinfo>
#include <vector>
#include <map>
#include <array>
#include <list>
#include <set>

#include "base/persistent_base.h"
#include "utils/city_hash.h"

using namespace std;

class TestA
{
public:
    int a;
    explicit TestA(int aa) { a = aa; };
    bool operator<(const TestA &other_ptr) const
    {
        return a < other_ptr.a;
    }
};

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
    void persist(const string &persisence_path) override{};

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    void resolve(const string &resolve_path) override{};
};

int main()
{
    map<SerializableA, SerializableB> m_map = {};
    m_map.emplace(SerializableA(1), SerializableB(1));
    m_map.emplace(SerializableA(2), SerializableB(2));
    Persistent p;
    bool dump_res = p.dump<SerializableA, SerializableB>(m_map, "/workspaces/dnfs/cmake-build-debug/test/base/tmp.bin");
    cout << "dump_res: " << dump_res << endl;

    map<SerializableA, SerializableB> new_map = {};
    bool load_res = p.load<SerializableA, SerializableB>("/workspaces/dnfs/cmake-build-debug/test/base/tmp.bin", &new_map);
    cout << "load_res: " << load_res << endl;
    if (load_res)
    {
        auto iter = new_map.end();
        iter--;
        cout << iter->first.a << "," << iter->second.b << endl;
    }

    cout << "----------" << endl;
    cout << typeid(int).name() << endl;
    cout << typeid(float).name() << endl;
    cout << typeid(TestA).name() << endl;
    cout << typeid(SerializableA).name() << endl;
    cout << typeid(SerializableB).name() << endl;
    cout << typeid(array<int, 6>).name() << endl;
    cout << typeid(vector<int>).name() << endl;
    cout << typeid(list<int>).name() << endl;
    cout << typeid(set<int>).name() << endl;
    cout << typeid(map<int, int>).name() << endl;
    cout << "----------" << endl;

    try
    {
        map<TestA, SerializableB> mm_map = {};
        mm_map.emplace(TestA(1), SerializableB(1));
        Persistent pp;
        bool pp_dump_res = pp.dump<TestA, SerializableB>(mm_map, "/workspaces/dnfs/cmake-build-debug/test/base/tmp_1.bin");
        cout << "dump_res: " << pp_dump_res << endl;
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }

    std::string s = "hello world";
    const char *ss = s.c_str();
    int l = strlen(ss);
    uint64 hash_res = CityHash64(ss, l);
    std::cout << "CityHash64: " << hash_res << std::endl;

    return 0;
}