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
#ifndef DNFSD_OBJECT_FILE_BASE_H
#define DNFSD_OBJECT_FILE_BASE_H

#include <map>
#include "base/serializable_base.h"

/*文件信息*/
enum FileInfoType {
    FILE_INFO,
    DIR_INFO
};

/*文件信息解释对应字典*/
static std::map<FileInfoType, std::string> info_explain_map = {{FILE_INFO, "file"},
                                                               {DIR_INFO,  "die"}};

/*文件信息操作*/
enum FileOperate {
    CREATE,
    DELETE,
    UPDATE,
    SELECT
};

/*文件信息基类*/
class ObjectInfoBase : public Serializable {
private:
    /*句柄指针*/
    struct ObjectHandle *object_handle;
    /*挂载卷文件系统路径*/
    std::string mvfs_path;
    /*文件名*/
    std::string name;
    /*文件io handle*/
    void *io_handle;
    /*todo 锁添加*/
    /*挂载卷的id*/
    std::string volvme_id;
    /*文件信息类型*/
    FileInfoType file_info_type;
public:
    /*生成信息保存路径*/
    std::string generate_file_save();
    /*todo 生成hash,將其傳給filehandle*/
};


#endif //DNFSD_OBJECT_FILE_BASE_H
