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

int main() {
    return 0;
}