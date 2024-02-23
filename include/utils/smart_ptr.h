/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#ifndef DNFSD_SMART_PTR_H
#define DNFSD_SMART_PTR_H

#include <mutex>
#include <map>
#include <memory>

/*使用智能指针都需要继承此类，实现获取id的方法*/
class SmartPtrValue {
public:
    /*删除数据虚函数*/
    virtual uint64_t get_id() = 0;

    /*相同类型数据对照方法*/
    virtual bool compore_data(SmartPtrValue *compare_value) = 0;
};

/*使用智能指针构建池管理器的都需要继承此类*/
class SmartPtrPool {
public:
    /*删除数据虚函数*/
    virtual void
    delete_item(const uint64_t &delete_key, SmartPtrValue *smart_ptr_value) = 0;
};

/*智能指针*/
class SmartPtr {
public:
    struct Pointers {
        /*保存的指针*/
        T *ptr;
        /*数据池指针*/
        SmartPtrPool *smart_ptr_pool;
    };
private:
    /* 所有的指针 */
    Pointers pointers;
    /*指针计数*/
    std::atomic<uint32_t> *count;

public:
    /*构造函数*/
    explicit SmartPtr(Pointers ptr_struct);

    /*拷贝构造函数*/
    SmartPtr(const SmartPtr &cp_ptr);

    /*计数器加1*/
    void add_count();

    /*返回引用计数*/
    unsigned int use_count();

    /*返回记录的数据指针*/
    SmartPtrValue *get_ptr();

    /*重载=，只有当两个ptr不相等时计数加1*/
    SmartPtr &operator=(const SmartPtr &cp_ptr);

    /*重载* */
    SmartPtrValue &operator*();

    /*重载->*/
    SmartPtrValue *operator->();

    /*重载==*/
    bool operator==(const SmartPtr &other_ptr);

    /*重载<*/
    bool operator<(const SmartPtr &other_ptr) const;

    /*释放内存*/
    void realse();

    /*析构函数*/
    ~SmartPtr();
};

/*构造函数*/
template<typename T>
SmartPtr<T>::SmartPtr(SmartPtr<T>::Pointers ptr_struct) {
    printf("调用构造函数");
    pointers = ptr;
    count = new std::atomic<uint32_t>(1);
}

/*拷贝构造函数*/
template<typename T>
SmartPtr<T>::SmartPtr(const SmartPtr<T> &cp_ptr) {
    /*复制参数*/
    ptr = cp_ptr.ptr;
    count = cp_ptr.count;
    printf("%p,%s\n", ptr, "调用拷贝构造");
    /*计数加1*/
    add_count();
}

/*计数器加1*/
template<typename T>
void SmartPtr<T>::add_count() {
    (*count)++;
    printf("%p,计数器为 %d\n", ptr, count->load());
}

/*重载=，只有当两个ptr不相等时计数加1*/
template<typename T>
unsigned int SmartPtr<T>::use_count() {
    return count->load();
}

/*释放内存*/
template<typename T>
void SmartPtr<T>::realse() {
    printf("%p,%s\n", ptr, "调用解析");
    /*如果需要计数为0*/
    if (--(*count) == 1) {
        printf("%p,%s\n", ptr, "真正释放");
        /*释放计数内存*/
        delete count;
        count = nullptr;
        /*释放内容内存*/
        delete ptr;
        ptr = nullptr;
        /*todo 从全局map删除其对应的数据*/
        smart_ptr_pool->delete_item(this);
    }
}

/*析构函数*/
template<typename T>
SmartPtr<T>::~SmartPtr() {
    printf("%p,%s\n", ptr, "调用析构");
    realse();
}


#endif //DNFSD_SMART_PTR_H
