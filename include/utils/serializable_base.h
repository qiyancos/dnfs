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
#ifndef DNFSD_SEQUENCE_BASE_H
#define DNFSD_SEQUENCE_BASE_H
#include "string"
/*todo 设计二进制缓存结构体*/
/*持久化基类*/
class Serializable {
public:
    Serializable();
protected:
    /*序列化*/
    virtual void serialize()=0;
    /*反序列化*/
    virtual void* deserialize()=0;
};


#endif //DNFSD_SEQUENCE_BASE_H
