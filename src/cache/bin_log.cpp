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
    last_log_file_path = (string) BIN_LOG_PATH + "/disk_" + file_name;;

    /*判断创建文件夹*/
    creat_directory(BIN_LOG_PATH);

    /*初始化日志文件*/
    init_log_file();

    /*初始化日志缓存*/
    memery_buffer = new LogBufferMap();
    /*todo 开启监控落盘线程*/
    watch_disk_work();

}

/*判断落盘日志文件是否存在*/
void BinLog::judge_last_file_exist() {
    if (access(last_log_file_path.c_str(), F_OK) == 0) {
        if (access(last_log_file_path.c_str(), R_OK) != 0) {
            /*判断文件读权限,没有说明不能进行数据恢复*/
            LOG(MODULE_NAME, EXIT_ERROR,
                "The last bin log file '%s' do not have read permission",
                last_log_file_path.c_str());
        }
        /*todo 开线程,进行数据恢复，将数据读到缓存后，直接关闭句柄，删除文件*/
        restore_data();
    }
}

/*在对象建立时判断工作日志文件是否存在，存在将数据读取到工作缓存，不存在创建*/
void BinLog::generate_work_file() {
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

        /*todo 将数据读到工作缓存*/
    }
}

/*创建日志文件，不存在创建，存在进行数据恢复，判断写权限，获取句柄，*/
void BinLog::init_log_file() {
    /*先判断是否存在落盘日志文件*/
    judge_last_file_exist();

    /*判断工作日志文件是否存在*/
    generate_work_file();
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
    }
}

/*写入缓存文件,将整个缓存写入*/
void BinLog::sync_log() {

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
bool BinLog::need_switch() {
    /*比大小时加锁*/
    unique_lock<mutex> buffer_l(buffer_lock);
    /*大于限制*/
    if (buffer_limit < memery_buffer->size()) {
        return true;
    }
    return false;
}

/*切换缓存log，封存当前log文件,创建新的文件并获取句柄*/
void BinLog::switch_buffer() {
    /*只有当落盘缓存为空指针时才进行落盘，否则忽略此次罗盘*/
    if (disk_buffer == nullptr) {
        /*切换缓存指针*/
        disk_buffer = memery_buffer;

        /*切换缓存时需要加锁*/
        unique_lock<mutex> buffer_t(buffer_lock);

        /*关闭文件句柄*/
        close(bin_log_fd);

        /*将文件句柄置为-1*/
        bin_log_fd = -1;

        /*重命名文件*/
        if (rename(work_log_file_path.c_str(), last_log_file_path.c_str()) != 0) {
            LOG(MODULE_NAME, EXIT_ERROR, "Failed to rename '%s' to '%s'",
                work_log_file_path.c_str(),
                last_log_file_path.c_str());
        }

        /*重新建立日志文件*/
        init_log_file();

        /*重新分配缓存空间*/
        memery_buffer = (LogBufferMap *) gsh_calloc(1, sizeof(LogBufferMap));

        /*todo 线程进行缓存落盘*/
        statistics_disk_buffer();
    }
}

/*规划需要落盘的缓存*/
void BinLog::statistics_disk_buffer() {
    /*落盘整理信息保存map*/
    disk_map disk_m = {};
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
    /*todo 由真正的落盘管理器进行落盘，这里只负责推送*/
    /*将落盘监控标志设置为true*/
    watch_disk = true;
}

/*监控落盘是否完成 */
void BinLog::watch_disk_work() {
    /*每个文件开线程进行数据落盘，todo根据资源判断怎么开线程，现在是每个文件各开一个线程*/
    if (watch_disk) {
        /*todo 当真正落完盘,进行下列操作*/
        remove(last_log_file_path.c_str());
        /*释放保存的落盘数据*/
        delete disk_buffer;
        /*监控本次数据落完盘后监控标志置为false*/
        watch_disk = false;
    }
}

/*从日志文件恢复数据*/
void BinLog::restore_data() {
    /*todo 将数据读到落盘缓存*/

    /*统计缓存信息*/
    statistics_disk_buffer();
}

/*析构函数，释放buffer指针，关闭句柄*/
BinLog::~BinLog() {
    /*释放日志缓存*/
    delete memery_buffer;
    delete disk_buffer;
}
