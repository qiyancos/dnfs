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
#ifndef DNFSD_BIN_LOG_H
#define DNFSD_BIN_LOG_H

#include <map>
#include <vector>
#include <mutex>
#include "meta/object_handle.h"
#include "meta/object_info_base.h"
#include "base/persistent_base.h"
#include "lru.h"

/*日志缓存map,hash:<文件句柄，文件信息>*/
typedef std::map<ObjectHandle *, ObjectInfoBase *> LogBufferMap;

/*保存落盘统计信息*/
typedef std::map<std::string, std::vector<ObjectHandle *>> disk_map;

/*bin_log路劲*/
#define BIN_LOG_PATH "/var/lib/dnfs/bin_log"
/*bin_log名字*/
#define BIN_LOG_NAME "bin_log.bin"
/*日志缓存大小*/
#define BIN_LOG_BUFFER_LIMIT 100
/*记录工作缓存变化次数，查过进行落盘*/
#define BUFFER_CHANGE_LIMIT 50

/*创建单例模式，记录文件操作信息*/
class BinLog final : public PersistentBase {
private:
    /*缓存数据,用filehandle的hash作为索引，最新的文件信息指针，文件信息在日志文件的偏移量，便于修改*/
    LogBufferMap *memery_buffer = nullptr;

    /*存储需要罗盘的缓存数据*/
    LogBufferMap *disk_buffer = nullptr;

    /*写日志文件时的保存数据*/
    LogBufferMap *persist_buffer = nullptr;

    /*缓存文件io句柄*/
    int bin_log_fd = -1;

    /*文件信息标志，现在位目录信息和文件信息，1为目录信息，2为文件信息*/
    FileInfoType info_type;

    /*针对于日志缓存添加线程读写锁*/
    std::mutex buffer_lock;

    /*获取限制*/
    const int buffer_limit = BIN_LOG_BUFFER_LIMIT;

    /*日志文件名*/
    std::string file_name;
    /*日志文件路径*/
    std::string work_log_file_path;
    /*落盘日志文件保存路径*/
    std::string last_log_file_path;

    /*工作缓存变化次数记录*/
    int buffer_change_count;
    /*工作缓存变化次数限制，超过落盘*/
    int buffer_change_limit = BUFFER_CHANGE_LIMIT;

    /*是否监控落盘标志*/
    bool watch_disk = false;
public:
    /*构造函数,现在仅需传入文件类型，其他配置文件读取
     * params info_type:文件信息标志
     * */
    BinLog(FileInfoType info_type);

    /*反序列化尚未完成落盘日志文件*/
    void resolve_last_file();

    /*序列化工作日志文件*/
    void resolve_work_file();

    /*推送信息，增加操作字节追加缓存，删除修改进行合并
     * params obj_handle:操作句柄
     * params obj_info:需要记录的文件信息指针
     * */
    void push_info(ObjectHandle *obj_handle, ObjectInfoBase *obj_info);

    /*写入缓存文件,将整个信息写入*/
    void sync_log();

    /*读取缓存
     * params obj_handle:需要获取信息的句柄
     * return 获取的文件信息
     * */
    bool find_info(ObjectHandle *obj_handle, ObjectInfoBase *obj_info);

    /*缓存大小信息大小比对，只有追加文件信息才进行大小比对*/
    bool need_switch();

    /*切换缓存log，封存当前log文件,创建新的文件并获取句柄*/
    void switch_buffer();

    /*规划需要落盘的缓存*/
    void object_mapping(disk_map &disk_m);

    /*将落盘任务发送到落盘管理器
     * params disk_m:分发的任务数据
     * */
    void dispatch_task(const disk_map &disk_m);

    /*缓存落盘*/
    void persist_old_binlog();

    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path) override;

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    void resolve(const std::string &resolve_path) override;

    /*析构函数*/
    ~BinLog();

};


#endif //DNFSD_BIN_LOG_H
