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
#include "global_single/global_single.h"
#include "meta/object_handle_pool.h"
#include "utils/smart_ptr.h"

GloablSingle::GloablSingle() {
    /*初始化句柄管理池*/
    object_pool = new ObjectHandlePool();
    /*在这里通知智能指针全局管理器为此类*/

}
