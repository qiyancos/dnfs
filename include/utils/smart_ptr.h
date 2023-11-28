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

template<typename T>
class SmartPtr {
private:
    /*保存的指针*/
    T *ptr;
    /*指针计数*/
    int *count;
    /*计数锁*/
    std::mutex *count_mutex;
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

    /*重载* */
    T &operator*() {
        return *this->ptr;
    }

    /*重载->*/
    T *operator->() {
        return this->ptr;
    }

    /*释放内存*/
    void realse();

    /*析构函数*/
    ~SmartPtr();
};

/*构造函数*/
template<typename T>
SmartPtr<T>::SmartPtr(T *ptr) {
    this->ptr = ptr;
    count = new int(1);
    count_mutex = new std::mutex;
}

/*拷贝构造函数*/
template<typename T>
SmartPtr<T>::SmartPtr(const SmartPtr<T> &cp_ptr) {
    printf("%s\n", "调用拷贝构造");
    /*复制参数*/
    ptr = cp_ptr.ptr;
    count = cp_ptr.count;
    count_mutex = cp_ptr.count_mutex;
    /*计数加1*/
    add_count();
}

/*计数器加1*/
template<typename T>
void SmartPtr<T>::add_count() {
    /*加锁计数*/
    std::unique_lock<std::mutex> count_l(*count_mutex);
    (*count)++;
}

/*重载=，只有当两个ptr不相等时计数加1*/
template<typename T>
int &SmartPtr<T>::use_count() {
    return *count;
}

/*释放内存*/
template<typename T>
void SmartPtr<T>::realse() {
    printf("%s\n", "调用解析");
    /*设置删除标志*/
    bool delete_flag = false;
    /*如果需要计数为0*/
    if (--(*count) == 1) {
        printf("%s\n", "真正释放");
        std::unique_lock<std::mutex> count_l(*count_mutex);
        /*释放内存*/
        delete count;
        delete ptr;
        ptr = nullptr;
        count = nullptr;
        delete_flag = true;
    }
    /*删除锁空间*/
    if (delete_flag) {
        delete count_mutex;
        count_mutex = nullptr;
    }

}

/*析构函数*/
template<typename T>
SmartPtr<T>::~SmartPtr() {
    printf("%s\n", "调用析构");
    realse();
}


#endif //DNFSD_SMART_PTR_H
