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
#include <fcntl.h>
#include "cache/bin_log.h"
#include "utils/common_utils.h"
#include "log/log.h"

#define MODULE_NAME "bin_log"
using namespace std;

/*构造函数,现在仅需传入文件类型，其他配置文件读取
 * params info_type:文件信息标志
 * */
BinLog::BinLog(FileInfoType info_type) {
    /*保存文件信息类型*/
    this->info_type = info_type;

    /*由文件类型生成文件名*/
    file_name = info_explain_map[this->info_type] + "_" + BIN_LOG_NAME;

    /*拼接文件路径*/
    work_log_file_path = (string) BIN_LOG_PATH + "/" + file_name;
    /*日志文件落盘保存路径*/
    last_log_file_path = (string) BIN_LOG_PATH + "/disk_" + file_name;

    /*判断创建文件夹*/
    creat_directory(BIN_LOG_PATH);

    /*todo 开启监控缓存落盘线程*/
    sync_log();

    /*从日志文件反序列化数据*/
    resolve("");

    /*是否开启落盘线程*/
    if (!disk_buffer->empty()) {
        /*todo开启落盘线程*/
        cache_to_disk();
    }
}

/*判断落盘日志文件是否存在*/
void BinLog::resolve_last_file() {
    if (access(last_log_file_path.c_str(), F_OK) == 0) {
        if (access(last_log_file_path.c_str(), R_OK) != 0) {
            /*判断文件读权限,没有说明不能进行数据恢复*/
            LOG(MODULE_NAME, EXIT_ERROR,
                "The last bin log file '%s' do not have read permission",
                last_log_file_path.c_str());
        }
        /*todo 将数据读取到disk_buffer*/
    }
}

/*序列化工作日志文件*/
void BinLog::resolve_work_file() {
    /*创建日志文件并获取句柄，如果存在需要进行数据恢复*/
    if (access(work_log_file_path.c_str(), F_OK) != 0) {
        /*尝试打开文件*/
        bin_log_fd = open(work_log_file_path.c_str(), O_CREAT | O_WRONLY,
                          FILE_MODEL_644);
        /*如果文件流没有成功打开*/
        if (bin_log_fd == -1) {
            LOG(MODULE_NAME, EXIT_ERROR, "Failed to cerate bin log file '%s'",
                work_log_file_path.c_str());
        }
    } else {
        if (access(work_log_file_path.c_str(), R_OK & W_OK) != 0) {
            /*判断文件读写权限,没有说明不能进行数据恢复*/
            LOG(MODULE_NAME, EXIT_ERROR,
                "The bin log file '%s' do not have read permission",
                work_log_file_path.c_str());
        }

        /*获取追加写句柄*/
        bin_log_fd = open(work_log_file_path.c_str(), O_WRONLY | O_APPEND);
        /*如果文件流没有成功打开*/
        if (bin_log_fd == -1) {
            LOG(MODULE_NAME, EXIT_ERROR,
                "Failed to create bin log file '%s' after restore data",
                work_log_file_path.c_str());
        }

        /*todo 将数据读到work_buffer*/
    }
}

/*推送信息，增加操作字节追加缓存，删除修改进行合并
 * params obj_handle:操作句柄
 * params obj_info:需要记录的文件信息指针，当进行删除操作是应传入空指针
 * */
void BinLog::push_info(ObjectHandle *obj_handle, ObjectInfoBase *obj_info) {
    /*写缓存需要加锁*/
    unique_lock<mutex> buffer_l(buffer_lock);
    /*保存查找到的数据*/
    auto find_buffer = memery_buffer->find(obj_handle);
    /*如果句柄存在更新数据*/
    if (find_buffer != memery_buffer->end()) {
        /*存在更新句柄*/
        find_buffer->second = obj_info;
    } else {
        /*不存在插入句柄*/
        memery_buffer->insert({obj_handle, obj_info});
        /*缓存大小信息大小比对，只有追加文件信息才进行大小比对*/
        need_switch();
    }
    /*缓存变化次数加1*/
    buffer_change_count += 1;
}

/*写入缓存文件,将整个缓存写入*/
void BinLog::sync_log() {
    /*todo 开启数据落盘监控线程，调用persist函数，循环判定更改次数*/
    while (true) {
        /*说明需要落盘*/
        if (buffer_change_count > buffer_change_limit) {
            /*缓存落盘时需要加锁*/
            unique_lock<mutex> buffer_t(buffer_lock, defer_lock);
            /*加锁进行数据复制*/
            buffer_t.lock();
            /*清空记录数*/
            buffer_change_count = 0;
            /*todo 复制数据memery_buffer*/
            for (auto &data: *disk_buffer) {
                persist_buffer->insert({data.first, data.second});
            }
            /*释放锁*/
            buffer_t.unlock();
            /*todo 持久化数据*/
            persist("");
            /*删除复制数据*/
            delete persist_buffer;
        }
    }

}

/*读取缓存
 * params obj_handle:需要获取信息的句柄
 * return 获取的文件信息
 * */
bool BinLog::find_info(ObjectHandle *obj_handle, ObjectInfoBase *obj_info) {
    /*先置成空指针*/
    obj_info = nullptr;
    /*查找句柄信息*/
    auto find_buffer = memery_buffer->find(obj_handle);
    /*如果找到了，赋值指针return true*/
    if (find_buffer != memery_buffer->end()) {
        /*删除了就是空指针*/
        obj_info = find_buffer->second;
        return true;
    }
    return false;
}

/*缓存大小信息大小比对，只有追加文件信息才进行大小比对*/
void BinLog::need_switch() {
    /*比大小时加锁*/
    unique_lock<mutex> buffer_l(buffer_lock);
    /*大于限制*/
    if (buffer_limit < memery_buffer->size()) {
        switch_buffer();
    }
}

/*切换缓存log，封存当前log文件,创建新的文件并获取句柄*/
void BinLog::switch_buffer() {
    /*只有当落盘缓存为空指针时才进行落盘，否则忽略此次罗盘*/
    if (disk_buffer->empty()) {
        /*切换缓存指针*/
        disk_buffer = memery_buffer;

        /*切换缓存时需要加锁*/
        unique_lock<mutex> buffer_t(buffer_lock, defer_lock);

        buffer_t.lock();
        /*关闭文件句柄*/
        close(bin_log_fd);

        /*重命名文件*/
        if (rename(work_log_file_path.c_str(), last_log_file_path.c_str()) != 0) {
            LOG(MODULE_NAME, EXIT_ERROR, "Failed to rename '%s' to '%s'",
                work_log_file_path.c_str(),
                last_log_file_path.c_str());
        }

        /*重新建立日志文件*/
        bin_log_fd = open(work_log_file_path.c_str(), O_CREAT | O_WRONLY,
                          FILE_MODEL_644);
        /*如果文件流没有成功打开*/
        if (bin_log_fd == -1) {
            LOG(MODULE_NAME, EXIT_ERROR, "Failed to cerate bin log file '%s'",
                work_log_file_path.c_str());
        }

        /*重新分配缓存空间*/
        memery_buffer = new LogBufferMap();
        buffer_t.unlock();

        /*将数据进行整理落盘*/
        cache_to_disk();
    }
}

/*规划需要落盘的缓存
 * params disk_m:统计落盘缓存
 * */
void BinLog::statistics_disk_buffer(disk_map &disk_m) {
    /*创建统计信息保存map*/
    for (auto &data: *disk_buffer) {
        /*获取文件保存路径*/
        string save_path = data.second->generate_file_save();
        /*查询数据*/
        auto find_load = disk_m.find(save_path);
        /*判断数据是否存在*/
        if (find_load != disk_m.end()) {
            /*追加当前的数据*/
            find_load->second.push_back(data.first);
        } else {
            /*保存当前数据*/
            disk_m.insert({save_path, {data.first}});
        }
    }
}

/*将落盘任务发送到落盘管理器
 * params disk_m:分发的任务数据
 * */
void BinLog::dispatch_task(const disk_map &disk_m) {
    /*todo 将任务分发到落盘管理器，并监控任务的返回，如果有未完成的任务，重新规划数据进行分发，如果所有的分发都已经完成，删除文件和缓存*/


    /*删除保存的日志文件*/
    remove(last_log_file_path.c_str());
    /*清空缓存*/
    delete disk_buffer;
}

/*将缓存数据落盘*/
void BinLog::cache_to_disk() {
    /*落盘整理信息保存map*/
    disk_map disk_m = {};

    /*统计缓存信息,滨海推送到日志落盘管理器*/
    statistics_disk_buffer(disk_m);

    /*将落盘任务数据发送到落盘管理器*/
    dispatch_task(disk_m);
}

/*析构函数，释放buffer指针，关闭句柄*/
BinLog::~BinLog() {
    /*获取锁释放内存*/
    unique_lock<mutex> buffer_t(buffer_lock);
    /*进行日志落盘*/
    persist("");
    /*释放日志缓存*/
    delete memery_buffer;
    delete disk_buffer;
}


/*持久化
 * params path:持久化到的文件
 * */
void BinLog::persist(const std::string &persisence_path) {
    /*todo 将数据持久化到文件*/
}

/*读取持久化文件
* params path:读取的持久化文件
* */
void BinLog::resolve(const std::string &resolve_path) {
    /*反序列化尚未完成落盘日志文件*/
    resolve_last_file();

    /*序列化工作日志文件*/
    resolve_work_file();
}
