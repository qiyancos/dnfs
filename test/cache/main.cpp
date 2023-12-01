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

    SmartPtr *search_data(SmartPtrValue *ptr);
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

SmartPtr *DataList::search_data(SmartPtrValue *ptr) {
    for (auto &item: value) {
        if (ptr->compore_data(item.get_ptr())) {
            return &item;
        }
    }
    return nullptr;
}

class TestList final : public SmartPtrPool {
public:
    map<uint64_t, DataList *> test_map = {};
public:
    SmartPtr gett(const InsertData &st);

    SmartPtr push(SmartPtrValue *ptr);

    void delete_item(const uint64_t &delete_key, SmartPtrValue *smart_ptr_value) override;

    ~TestList();
};

SmartPtr TestList::gett(const InsertData &st) {
    /*直接构造句柄*/
    auto *in_data = new InsertData(st);
    /*查询数据是否存在*/
    auto item = test_map.find(in_data->get_id());
    /*保存查找到的数据*/
    SmartPtr *find_ptr = nullptr;
    if (item != test_map.end()) {
        /*遍历查找数据*/
        find_ptr = item->second->search_data(in_data);
        /*如果找到数据直接返回*/
        if (find_ptr) {
            printf("直接找到\n");
            delete in_data;
            return *find_ptr;
        }
    }
    /*插入后在返回*/
    return push(in_data);
}

SmartPtr TestList::push(SmartPtrValue *ptr) {
    /*先判定hashmap是否存在*/
    auto item = test_map.find(ptr->get_id());
    if (item != test_map.end()) {
        printf("往后插入\n");
        item->second->push(ptr, this);
        return *item->second->value.rbegin();
    } else {
        printf("直接创建\n");
        auto *da = new DataList(ptr, this);
        test_map.emplace(ptr->get_id(), da);
        return *da->value.begin();
    }
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
    printf("______________________\n");
    SmartPtr j = test_list.gett(s);
    printf("______________________\n");
    SmartPtr n = test_list.gett(InsertData(1, "1111"));
    printf("______________________\n");
    SmartPtr w = test_list.gett(InsertData(2, "111"));
    printf("______________________\n");

//    printf("%d\n",ptr_4->second);
//    thread t1(test_lock, 1);
//    thread t2(test_lock, 2);
//    t1.join();
//    t2.join();
    return 0;
}