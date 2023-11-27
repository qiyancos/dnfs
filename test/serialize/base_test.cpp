#include <ctime>
#include <fstream>
#include <iostream>
#include "cereal/archives/binary.hpp"
#include "cereal/types/map.hpp"

struct Inner
{
    int ia, ib, ic, id;

    template <class Archive>
    void serialize(Archive &ar)
    {
        /*需要序列化的字段*/
        ar(ia, ib, ic, id);
    }
};

struct MyRecord
{
    int a, b, c;
    // Inner inner;

    template <class Archive>
    void serialize(Archive &ar)
    {
        /*需要序列化的字段*/
        // ar(a, b, c);
        ar(a);
        ar(b);
        ar(c);
        // ar(a, b, c, inner);
    }
};

// typedef typename std::map<int, MyRecord> myMap;
typedef typename std::map<int, int> myMap;

void dumps(const myMap my_map)
{
    std::ofstream os("my_test.cereal", std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    archive(my_map);
}

void loads()
{
    std::ifstream is("my_test.cereal", std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    myMap my_map;
    archive(my_map);
    // for (const auto &i : my_map)
    // {
    //     std::cout << i.first << std::endl;
    //     std::cout << i.second.x << ", " << i.second.y << ", " << i.second.z << std::endl;
    // }
    // auto iter = my_map.end();
    // iter--;
    // std::cout << iter->first << std::endl;
    // std::cout << iter->second.inner.ia << std::endl;
}

double per_cost(clock_t s, clock_t e, int nums)
{
    return (double)(e - s) / CLOCKS_PER_SEC / nums;
}

int testMyMap(myMap my_map, int loop)
{
    // std::cout << "sizeof(char)=" << sizeof(char) << std::endl;
    // std::cout << "sizeof(int)=" << sizeof(int) << std::endl;
    // std::cout << "sizeof(float)=" << sizeof(float) << std::endl;
    // myMap my_map = {};
    // // MyRecord mr = {1, 2, 3.0};
    // // my_map.emplace(100, mr);
    // for (int i = 0; i < 2000; i++)
    // {
    //     // my_map.emplace(
    //     //     10 * i,
    //     //     (MyRecord){i + 1, i + 2, i + 3});
    //     // (MyRecord){i + 1, i + 2, i + 3, (Inner){i * 10, i * 20, i * 30, i * 40}});
    //     my_map.emplace(i, i*2);
    // }

    // std::cout << "-----------obj dumps-----------\n";
    clock_t dumps_start = clock();
    for (int i = 0; i < loop; i++)
    {
        dumps(my_map);
    }
    clock_t dumps_end = clock();
    std::cout << "dumps: " << per_cost(dumps_start, dumps_end, loop) << "s" << std::endl;

    // std::cout << "-----------obj loads-----------\n";
    clock_t loads_start = clock();
    for (int i = 0; i < loop; i++)
    {
        loads();
    }
    clock_t loads_end = clock();
    std::cout << "loads: " << per_cost(loads_start, loads_end, loop) << "s" << std::endl;

    return 0;
}

class SerializeBase
{
private:
    int a, b, c;

public:
    template <class Archive>
    void serialize(Archive &ar)
    {
        /*需要序列化的字段*/
        // ar(a, b, c);
        ar(a);
        ar(b);
        ar(c);
    };

    SerializeBase(int x, int y, int z)
    {
        a = x;
        b = y;
        c = z;
    };

    SerializeBase(){};

    void dumps()
    {
        std::ofstream os("serialize.cereal", std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(*this);
    };

    void loads()
    {
        std::ifstream is("serialize.cereal", std::ios::binary);
        cereal::BinaryInputArchive archive(is);
        archive(*this);
    };

    void print()
    {
        std::cout << "a: " << this->a << std::endl;
        std::cout << "b: " << this->b << std::endl;
        std::cout << "c: " << this->c << std::endl;
    }
};

int testSerializeBase()
{
    SerializeBase tmp(4, 2, 3);
    tmp.dumps();

    SerializeBase n_tmp;
    n_tmp.loads();
    n_tmp.print();

    return 0;
};

int main()
{
    std::cout << "cereal..." << std::endl;
    myMap my_map = {};
    int loop = 1000;
    for (int d = 0; d < 10; d++)
    {
        int i = d * 2000;
        for (int j = 0; j < 2000; j++)
        {
            my_map.emplace(i, i);
            i += 1;
        }
        std::cout << "map_size: " << my_map.size() << " loop num: " << loop << std::endl;
        testMyMap(my_map, loop);
    }

    return 0;
}