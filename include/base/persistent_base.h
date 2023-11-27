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
#ifndef DNFSD_PERSISTENT_BASE_H
#define DNFSD_PERSISTENT_BASE_H

#include <string>
#include <map>
#include <vector>

/*持久化数据*/
class PersistentBase {
public:
    /*持久化
     * params path:持久化到的文件
     * */
    virtual void persist(const std::string &persisence_path) = 0;

    /*读取持久化文件
    * params path:读取的持久化文件
    * */
    virtual void resolve(const std::string &resolve_path) = 0;
};


#endif //DNFSD_PERSISTENT_BASE_H
