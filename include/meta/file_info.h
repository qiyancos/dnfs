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
#ifndef DNFSD_FILE_INFO_H
#define DNFSD_FILE_INFO_H

#include <string>
#include <atomic>
#include "object_info_base.h"

/*持久化基类，序列化和反序列化*/
/*包含了文件信息 */

class FileInfo : public ObjectInfoBase {
    /*硬链接计数*/
    std::atomic<int> hard_link_count = 0;
};


#endif //DNFSD_FILE_INFO_H
