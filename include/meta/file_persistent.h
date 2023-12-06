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
#ifndef DNFSD_FILE_PERSISTENT_H
#define DNFSD_FILE_PERSISTENT_H

#include "base/persistent_base.h"
#include "object_handle.h"

class FilePersistent : public PersistentBase {
public:
    /*在启动时初始化name path map*/
    void init_name_path_map();

    /*根据句柄hash判断保存信息的文件路径*/
    std::string generte_save_path(ObjectHandle *object_handle);

    /*查询是否存在持久化的文件
     * params file_hash:文件的hsah值
     * params save_path:文件句柄保存路劲
     * return 是否存在文件夹
     * */
    void serch_save_file(const std::string &file_hash, std::string *save_path);

    /*创建保存文件*/
    void build_save_file(const std::string &file_path, std::string *save_path);

    /*合并写入信息，减少写io，将同个文件的信息进行合并
     * params file_handle:待操作的句柄
     * params what_do:进行的操作
     * */
    void merge_messge(const ObjectHandle *obj_handle, short what_do);

    /*合并文件
     * params first_path:待合并文件
     * params second_path:待合并文件
     * */
    void marge_file(std::string first_path, std::string second_path);

    /*拆分文件
     * params split_path:拆分文件路径
     * */
    void split_file(std::string split_path);

    /**/
};


#endif //DNFSD_FILE_PERSISTENT_H
