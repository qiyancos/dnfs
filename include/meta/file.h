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
#ifndef DNFSD_FILE_H
#define DNFSD_FILE_H
#include <string>
#include "file_handle.h"
#include "utils/serializable_base.h"

/*持久化基类，序列化和反序列化*/
/*包含了文件信息 */

class file : public Serializable{
    /*句柄指针*/

    /*挂载卷文件系统路径*/
    std::string mvfs_path;
    /*文件名*/
    std::string name;
    /*硬链接计数*/
    std::atomic<int> hard_link_count = 0;
    /*文件io handle*/
    void *io_handle;
    /*todo 锁添加*/
    /*挂载卷的id*/
    std::string volvme_id;
};


#endif //DNFSD_FILE_H
