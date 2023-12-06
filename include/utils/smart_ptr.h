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
    struct Ptrs {
        /*保存的指针*/
        SmartPtrValue *ptr = nullptr;
        /*数据池指针*/
        SmartPtrPool *smart_ptr_pool = nullptr;

        /*构造函数*/
        Ptrs(SmartPtrValue *ptr, SmartPtrPool *smart_ptr_pool);
    };

private:
    SmartPtrValue *ptr;
    /*数据池指针*/
    SmartPtrPool *smart_ptr_pool;
    /*指针计数*/
    std::atomic<uint32_t> *count;

public:
    /*构造函数*/
    explicit SmartPtr(const Ptrs &ptrs);

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

#endif //DNFSD_SMART_PTR_H
