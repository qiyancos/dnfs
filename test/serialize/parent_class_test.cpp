#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include "cereal/archives/binary.hpp"
#include "cereal/types/map.hpp"

class SerializeBase
{
private:
public:
    // template <class Archive>
    // void serialize(Archive &ar)
    // {
    //     /*需要序列化的字段*/
    //     std::cout << "SerializeBase serialize" << std::endl;
    // };

    void dumps(std::string bin_filepath)
    {
        std::ofstream os(bin_filepath, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(*this);
    };

    void loads(std::string bin_filepath)
    {
        std::ifstream is(bin_filepath, std::ios::binary);
        cereal::BinaryInputArchive archive(is);
        archive(*this);
    };
};

class Serialize : public SerializeBase
{
private:
    std::map<int, int> m;

public:
    void set(int k, int v)
    {
        m.emplace(k, v);
    }

    void print()
    {
        for (auto &item : this->m)
        {
            std::cout << "(" << item.first << ", " << item.second << ")" << std::endl;
        }
    };

    template <class Archive>
    void serialize(Archive &ar)
    {
        /*需要序列化的字段*/
        ar(m);
        // std::cout << "serialize" << std::endl;
    };

    // void dumps(std::string bin_filepath)
    // {
    //     std::ofstream os(bin_filepath, std::ios::binary);
    //     cereal::BinaryOutputArchive archive(os);
    //     archive(*this);
    // };

    // void loads(std::string bin_filepath)
    // {
    //     std::ifstream is(bin_filepath, std::ios::binary);
    //     cereal::BinaryInputArchive archive(is);
    //     archive(*this);
    // };
};

// CEREAL_REGISTER_TYPE(Serialize);
// CEREAL_REGISTER_TYPE(SerializeBase);
// CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializeBase, Serialize)

int main()
{
    Serialize s;
    s.set(1, 1);
    s.set(2, 2);
    s.dumps("s.bin");
    s.print();

    std::cout << "----------" << std::endl;

    Serialize ss;
    ss.loads("s.bin");
    ss.print();

    return 0;
}
