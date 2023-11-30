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
#include "cache/bin_log.h"
#include <cstring>
#include <mutex>
#include <thread>
#include <ctime>
#include <map>
#include "utils/smart_ptr.h"
#include <memory>
#include <chrono>
#include <utility>
#include <ctime>

using namespace std;

/*需要添加的数据*/
class InsertData final : public SmartPtrValue {
private:
    uint64_t data_id;
    string data;
public:
    /*构造函数*/
    explicit InsertData(uint64_t data_id, string data) : data_id(data_id),
                                                         data(std::move(data)) {}

    /*重写获取id的方法*/
    uint64_t get_id() override;

    bool compore_data(SmartPtrValue *compare_value) override;

    string get_data();
};

uint64_t InsertData::get_id() {
    return data_id;
}

bool InsertData::compore_data(SmartPtrValue *compare_value) {
    auto *com = (InsertData *) compare_value;
    return data == com->get_data();
}

string InsertData::get_data() {
    return data;
}


/*存储数据的list*/
struct DataList {
    std::list<SmartPtr> value;
public:
    explicit DataList(SmartPtrValue *tr, SmartPtrPool *pool);

    void push(SmartPtrValue *tr, SmartPtrPool *pool);

    void delete_data(SmartPtrValue *tr);
};

DataList::DataList(SmartPtrValue *tr, SmartPtrPool *pool) {
    value.emplace_back(SmartPtr::Ptrs({tr, pool}));
}

void DataList::push(SmartPtrValue *tr, SmartPtrPool *pool) {
    value.emplace_back(SmartPtr::Ptrs({tr, pool}));
}

void DataList::delete_data(SmartPtrValue *tr) {
    for (auto &item: value) {
        if (tr->compore_data(item.get_ptr())) {
            /*释放数据*/
            item.realse();
        }
    }
}

class TestList final : public SmartPtrPool {
public:
    map<uint64_t, DataList *> test_map = {};
public:
    void gett(const int &key);

    void push(const int &data, InsertData &st);

    void delete_item(const uint64_t &delete_key, SmartPtrValue *smart_ptr_value) override;

    ~TestList();
};

void TestList::gett(const int &key) {
    auto s = test_map.find(key)->second->value;
    auto w = s.end();
}

void TestList::push(const int &data, InsertData &st) {

    auto *inda = new InsertData(st);

    auto item = test_map.find(data);
    if (item != test_map.end()) {
        item->second->push(inda, this);
    } else {
        auto *da = new DataList(inda, this);
        test_map.emplace(data, da);
    }
    printf("_________");
}

void TestList::delete_item(const uint64_t &delete_key, SmartPtrValue *smart_ptr_value) {
    printf("删除的id:%lu\n", delete_key);
    /*先获取需要删除的数据*/
    auto d_data = test_map.find(delete_key);
    if (d_data != test_map.end()) {
        printf("删除数据\n");
        d_data->second->delete_data(smart_ptr_value);
    }
    test_map.erase(delete_key);
}

TestList::~TestList() {
    for (auto s: test_map) {
        delete s.second;
    }

}


mutex whar;
int i = 0;

void test_lock(int s) {
    while (i < 100) {
        {
            unique_lock<mutex> tt(whar);
            i++;
            printf("id:%d,sss%d\n", s, i);
        }
        unique_lock<mutex> ww(whar);
        printf("id:%d,www%d\n", s, i);
    }
}

class test {
public:
    int tes();
};

int test::tes() {
    return 0;
}


int main() {
//    printf("%lu\n", sizeof(SmartPtr));
//    PoolTest pool_t = PoolTest();
//    PoolTest pool_3 = PoolTest();
//    printf("%p\n", &pool_3);
//    printf("%p\n", &pool_t);
//    insert(pool_t);
//    get(pool_t, pool_3);


    TestList test_list = TestList();
    InsertData s = InsertData(1, "111");
    test_list.push(1, s);
    printf("______________________\n");
    test_list.gett(1);

//    printf("%d\n",ptr_4->second);
//    thread t1(test_lock, 1);
//    thread t2(test_lock, 2);
//    t1.join();
//    t2.join();
    return 0;
}