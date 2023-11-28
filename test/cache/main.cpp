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
#include <ptr
#include "utils/smart_ptr.h"
#include <memory>

using namespace std;
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

int main() {
    SmartPtr<string> ptr_1(new string("1111"));
    printf("%d\n", ptr_1.use_count());
    SmartPtr<string> ptr_2 = ptr_1;
    printf("%d\n", ptr_2.use_count());

    map<SmartPtr<string>, int> map_test;
    map_test.emplace(ptr_1, 2);
    map_test.emplace(ptr_2, 3);

    auto ptr_3 = map_test.find(ptr_1);
//    thread t1(test_lock, 1);
//    thread t2(test_lock, 2);
//    t1.join();
//    t2.join();

    return 0;
}