/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "base/persistent_base.h"

#include <cstring>

using namespace std;

class A final : public Serializable
{
    int a;

public:
    explicit A(int aa) { a = aa; };

protected:
    /*序列化*/
    void serialize() override{};

    /*反序列化*/
    void *deserialize() override{ return nullptr; };
};

class B final : public Serializable
{
    float b;

public:
    explicit B(float bb) { b = bb; };

protected:
    /*序列化*/
    void serialize() override{};

    /*反序列化*/
    void *deserialize() override{ return nullptr; };
};

template <typename KEY, typename VALUE>
class Persistent : public PersistentBase<KEY, VALUE>
{
public:
    Persistent(){};
    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path){};

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    void resolve(const std::string &resolve_path){};
};

int main()
{
    map<A*, B*> m_map = {};
    A a=A(1);
    B b=B(1.1);
    m_map.emplace(&a, &b);

    Persistent<A*,B*> p;
    p.save_to_file(m_map, "tmp.bin");
    return 0;
}