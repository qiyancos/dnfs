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


using namespace std;

class PoolTest final : public SmartPtrPool {
public:
    map<SmartPtr<string> *, SmartPtr<string>> map_test;
public:
    /*得到数据*/
    SmartPtr<string> get(SmartPtr<string> *t);

    /*存储数据*/
    void push(const SmartPtr<string> &t, SmartPtr<string> *data);

    void delete_item(void *key) override;
};

SmartPtr<string> PoolTest::get(SmartPtr<string> *t) {
    auto item = map_test.find(t);
    return item->second;
}

void PoolTest::push(const SmartPtr<string> &t, SmartPtr<string> *data) {
    auto item = map_test.find(data);
    if (item != map_test.end()) {
        item->second = t;
    } else {
        map_test.emplace(data, t);
    }
}

void PoolTest::delete_item(void *key) {
    printf("delete\n");
    map_test.erase((SmartPtr<string>*)key);
}


void insert(PoolTest &pool_test) {
    SmartPtr<string> ptr_1(new string("1111"), &pool_test);
    printf("-------------------------------\n");
    SmartPtr<string> ptr_2(new string("111s"), &pool_test);
//    printf("-------------------------------\n");
//    pool_test.map_test.emplace(3, SmartPtr<string>(new string("111111"), &pool_test));
//    printf("-------------------------------\n");
//    pool_test.map_test.emplace(4, new string("111111"), &pool_test);
//    printf("-------------------------------\n");
//    ptr_1=ptr_2;
    pool_test.push(ptr_1, &ptr_1);
    printf("-------------------------------\n");
    pool_test.push(ptr_2, &ptr_2);
    printf("-------------------------------\n");
    printf("insert end\n");
}

void get(PoolTest &pool_test, PoolTest &pool_3) {
//    printf("get data\n");
//    SmartPtr<string> ptr_1 = pool_test.get(1);
//    printf("-------------------------------\n");
//    SmartPtr<string> ptr_2 = pool_test.get(2);
//    printf("-------------------------------\n");
//    SmartPtr<string> ptr_3 = pool_test.get(3);
//    printf("-------------------------------\n");
//    SmartPtr<string> ptr_4 = pool_test.get(4);
//    printf("-------------------------------\n");
//    printf("插入第二个pool\n");
//    pool_3.push(ptr_1, 1);
//    printf("-------------------------------\n");
//    pool_3.push(ptr_2, 2);
//    printf("-------------------------------\n");
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
    printf("%lu\n", sizeof(SmartPtr<string>));
    PoolTest pool_t = PoolTest();
    PoolTest pool_3 = PoolTest();
    insert(pool_t);
    get(pool_t, pool_3);
//    printf("what fuck %zu\n",map_test.size());
//
//    auto ptr_3 = map_test.find(ptr_1);
//    if(ptr_3==map_test.end()){
//        printf("what fuck %zu\n",map_test.size());
//    }
//    auto ptr_4 = map_test.find(ptr_2);
//    if(ptr_4==map_test.end()){
//        printf("what fuck\n");
//    }
//    printf("%d\n",ptr_4->second);
//    thread t1(test_lock, 1);
//    thread t2(test_lock, 2);
//    t1.join();
//    t2.join();
    return 0;
}