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
#ifndef DNFSD_SMART_PTR_H
#define DNFSD_SMART_PTR_H

#include <mutex>
#include <map>
template<typename T>
class SmartPtr {
private:
    /*保存的指针*/
    T *ptr;
    /*指针计数*/
    int *count;
    /*计数锁*/
    std::mutex *count_mutex;
    /*计数变化标志，防止只初始化了一次，导致没有释放内存*/
    bool *count_change;
    /*已经删除标志*/
    bool *is_delete;
    /*在初始化的数据后第一次析构啥都不做*/
    bool *is_build;

public:
    /*构造函数*/
    explicit SmartPtr(T *ptr);

    /*拷贝构造函数*/
    SmartPtr(const SmartPtr<T> &cp_ptr);

    /*计数器加1*/
    void add_count();

    /*重载=，只有当两个ptr不相等时计数加1*/
    SmartPtr<T> &operator=(const SmartPtr<T> &cp_ptr) {
        /*判断是不是同一个指针*/
        if (ptr != cp_ptr.ptr) {
            /*释放当前的内存*/
            realse();
            /*复制参数*/
            ptr = cp_ptr.ptr;
            count = cp_ptr.count;
            count_mutex = cp_ptr.count_mutex;
            /*计数加1*/
            add_count();
        }
        return *this;
    }

    /*返回引用计数*/
    int &use_count();

    /*返回是否已经删除数据*/
    bool get_delete();

    /*重载* */
    T &operator*() {
        return *this->ptr;
    }

    /*重载->*/
    T *operator->() {
        return this->ptr;
    }

    /*重载==*/
    bool operator==(const SmartPtr<T> &other_ptr) {
        return ptr == other_ptr.ptr;
    }

    /*重载<*/
    bool operator<(const SmartPtr<T> &other_ptr) const {
        return ptr < other_ptr.ptr;
    }

    /*释放内存*/
    void realse();

    /*析构函数*/
    ~SmartPtr();
};

/*构造函数*/
template<typename T>
SmartPtr<T>::SmartPtr(T *ptr) {
    printf("%p,调用构造函数\n", ptr);
    this->ptr = ptr;
    count = new int(0);
    count_mutex = new std::mutex;
    count_change = new bool(false);
    is_delete = new bool(false);
    is_build = new bool(true);
}

/*拷贝构造函数*/
template<typename T>
SmartPtr<T>::SmartPtr(const SmartPtr<T> &cp_ptr) {
    /*复制参数*/
    ptr = cp_ptr.ptr;
    count = cp_ptr.count;
    count_mutex = cp_ptr.count_mutex;
    count_change = cp_ptr.count_change;
    is_delete = cp_ptr.is_delete;
    is_build = cp_ptr.is_build;
    printf("%p,%s\n", ptr, "调用拷贝构造");
    /*计数加1*/
    add_count();
}

/*计数器加1*/
template<typename T>
void SmartPtr<T>::add_count() {
    /*count数量变更标志*/
    *count_change = true;
    /*加锁计数*/
    std::unique_lock<std::mutex> count_l(*count_mutex);
    (*count)++;
    printf("%p,计数器为 %d\n", ptr, *count);
}

/*重载=，只有当两个ptr不相等时计数加1*/
template<typename T>
int &SmartPtr<T>::use_count() {
    return *count;
}

/*返回是否已经删除数据*/
template<typename T>
bool SmartPtr<T>::get_delete() {
    return *is_delete;
}

/*释放内存*/
template<typename T>
void SmartPtr<T>::realse() {
    printf("%p,%s\n", ptr, "调用解析");
    {
        std::unique_lock<std::mutex> count_l(*count_mutex);
        /*如果需要计数为0*/
        if (--(*count) == 1 or !*count_change) {
            printf("%p,%s\n", ptr, "真正释放");
            /*释放计数内存*/
            delete count;
            count = nullptr;

            /*释放内容内存*/
            delete ptr;
            ptr = nullptr;

            /*设置删除标志*/
            *is_delete = true;
            return;
        }
        if (count != nullptr) {
            printf("%p,现在的count %d\n", ptr, *count);
        }
    }
    /*说明这是最后一个指针，进行删除剩余空间操作*/
    if (*is_delete) {
        {
            printf("彻底删除\n");
            std::unique_lock<std::mutex> count_l(*count_mutex);
            /*归还删除标志位空间*/
            delete is_delete;
            is_delete = nullptr;

            /*归还计数变化空间*/
            delete count_change;
            count_change = nullptr;
        }
        printf("删除锁空间\n");
        delete count_mutex;
        count_mutex = nullptr;
    }

}

/*析构函数*/
template<typename T>
SmartPtr<T>::~SmartPtr() {
    printf("%p,%s\n", ptr, "调用析构");
    if (!*is_build) {
        realse();
    }
    *is_build = false;
}


#endif //DNFSD_SMART_PTR_H
