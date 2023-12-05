/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Piyuyang pi_yuyang@163.com
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
#ifndef DNFSD_JOB_EXECUTE_H
#define DNFSD_JOB_EXECUTE_H

#include <cstdint>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <vector>

// 工作封装类
class JobBase
{
public:
    // 唯一标识
    uint64_t job_id;
    // 是否可重复
    bool repeat;

    /*执行*/
    // virtual bool execute() = 0;
    virtual bool execute()
    {
        std::cout << "execute in JobBase" << std::endl;
        return false;
    };
};

// 落盘任务执行器
class JobExecute
{
private:
    // 执行线程最大数量
    int _max_thread_size;

    // 工作队列
    std::list<JobBase> job_list = {};

public:
    // 初始化
    JobExecute(const int max_thread_size)
    {
        _max_thread_size = max_thread_size;
    }

    // 添加job
    uint64_t add(JobBase *job);

    // 查询job状态
    void job_info(std::vector<uint64_t> job_ids);

    // 删除job
    bool remove(std::vector<uint64_t> job_ids);

    // 执行job
    void execute(JobBase *job);

    // 扫描job_list
    void scan();

    // 主函数
    void main();
};

#endif // DNFSD_JOB_EXECUTE_H
