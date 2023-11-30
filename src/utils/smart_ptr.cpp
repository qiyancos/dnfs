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
#include "utils/smart_ptr.h"
#include <atomic>

/*构造函数*/
SmartPtr::SmartPtr(const Ptrs &ptrs) {
    ptr = ptrs.ptr;
    printf("%p,调用构造函数\n", ptr);
    smart_ptr_pool = ptrs.smart_ptr_pool;
    count = new std::atomic<uint32_t>(1);
}

/*拷贝构造函数*/
SmartPtr::SmartPtr(const SmartPtr &cp_ptr) {
    /*复制参数*/
    ptr = cp_ptr.ptr;
    count = cp_ptr.count;
    smart_ptr_pool = cp_ptr.smart_ptr_pool;
    printf("%p,%s\n", ptr, "调用拷贝构造");
    /*计数加1*/
    add_count();
}

/*计数器加1*/
void SmartPtr::add_count() {
    (*count)++;
    printf("%p,计数器为 %d\n", ptr, count->load());
}

/*重载=，只有当两个ptr不相等时计数加1*/
unsigned int SmartPtr::use_count() {
    return count->load();
}

/*返回记录的数据指针*/
SmartPtrValue *SmartPtr::get_ptr() {
    return ptr;
}

/*重载=，只有当两个ptr不相等时计数加1*/
SmartPtr &SmartPtr::operator=(const SmartPtr &cp_ptr) {
    printf("======\n");
    /*判断是不是同一个指针*/
    if (ptr != cp_ptr.ptr and cp_ptr.ptr != nullptr) {
        /*释放当前的内存*/
        realse();
        /*复制参数*/
        ptr = cp_ptr.ptr;
        count = cp_ptr.count;
        /*计数加1*/
        add_count();
    }
    return *this;
}

/*重载* */
SmartPtrValue &SmartPtr::operator*() {
    printf("**\n");
    return *this->ptr;
}

/*重载->*/
SmartPtrValue *SmartPtr::operator->() {
    return this->ptr;
}

/*重载==*/
bool SmartPtr::operator==(const SmartPtr &other_ptr) {
    return ptr == other_ptr.ptr;
}

/*重载<*/
bool SmartPtr::operator<(const SmartPtr &other_ptr) const {
    return ptr < other_ptr.ptr;
}

/*释放内存*/
void SmartPtr::realse() {
    printf("%p,%s\n", ptr, "调用解析");

    /*先进行计数自减*/
    --(*count);
    if (count->load() == 1) {
        smart_ptr_pool->delete_item(ptr->get_id(), ptr);
    } else if ((count->load() == 0)) {
        printf("%p,%s\n", ptr, "真正释放");
        /*释放计数内存*/
        delete count;
        /*释放内容内存*/
        delete ptr;
    }
}

/*析构函数*/
SmartPtr::~SmartPtr() {
    printf("%p,%s,%d\n", ptr, "调用析构", count->load());
    realse();
}

/*构造函数*/
SmartPtr::Ptrs::Ptrs(SmartPtrValue *ptr, SmartPtrPool *smart_ptr_pool) : ptr(ptr),
                                                                         smart_ptr_pool(
                                                                                 smart_ptr_pool) {

}
