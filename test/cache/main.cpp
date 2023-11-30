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
#include <ctime>

using namespace std;

class PoolTest final : public SmartPtrPool {
public:
    map<int, SmartPtr<string>> map_test;
    map<SmartPtr<string>, int> map_reverse;
public:
    /*得到数据*/
    SmartPtr<string> get(const int &t);

    /*存储数据*/
    void push(const int &data, const string &value);

    void delete_item(void *key) override;
};

SmartPtr<string> PoolTest::get(const int &t) {
    auto item = map_test.find(t);
    return item->second;
}

void PoolTest::push(const int &data, const string &value) {
    auto new_value_ptr = new string(value);
    printf("%p\n", this);
    Ptrs p = {new_value_ptr, this};
    map_test.emplace(data, p);
    map_reverse.emplace(get(data),data);
}

void PoolTest::delete_item(void *key) {
    printf("%p\n", key);
    printf("delete\n");
}


void insert(PoolTest &pool_test) {
    Ptrs p1 = {new string("1111"), &pool_test};
    SmartPtr<string> ptr_1({new string("1111"), &pool_test});
    printf("-------------------------------\n");
    Ptrs p2 = {new string("1111"), &pool_test};
    SmartPtr<string> ptr_2({new string("1111"), &pool_test});
    printf("-------------------------------\n");
    /**/
    Ptrs p3 = {new string("111111"), &pool_test};
    pool_test.map_test.emplace(3, p3);
    printf("-------------------------------\n");
//    pool_test.map_test.emplace(4, {new string("111111"), &pool_test});
//    printf("-------------------------------\n");
//    ptr_1=ptr_2;
    pool_test.push(1, "test1");
    printf("-------------------------------\n");
    pool_test.push(2, "test2");
    printf("-------------------------------\n");
    printf("insert end\n");
}

void get(PoolTest &pool_test, PoolTest &pool_3) {
    printf("get data\n");
    SmartPtr<string> ptr_1 = pool_test.get(1);
    printf("-------------------------------\n");
    SmartPtr<string> ptr_2 = pool_test.get(2);
    printf("-------------------------------\n");
    SmartPtr<string> ptr_3 = pool_test.get(3);
    printf("-------------------------------\n");
    SmartPtr<string> ptr_4 = pool_test.map_reverse.find(ptr_1)->first;
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
    printf("%p\n", &pool_3);
    printf("%p\n", &pool_t);
    insert(pool_t);
    get(pool_t, pool_3);
    /*获取毫秒*/
//    auto now = chrono::system_clock::now();
//    uint64_t microseconds = chrono::duration_cast<chrono::microseconds>(
//            now.time_since_epoch()).count();
//    uint64_t naroseconds = chrono::duration_cast<chrono::nanoseconds>(
//            now.time_since_epoch()).count();
//    printf("%lu\n",microseconds);
//    printf("%lu\n",naroseconds);
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