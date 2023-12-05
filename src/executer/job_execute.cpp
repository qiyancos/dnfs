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
#include "executer/job_execute.h"

uint64_t JobExecute::add(JobBase *job)
{
    uint64_t res = 1;
    return res;
}

void JobExecute::job_info(std::vector<uint64_t> job_ids) {}

bool JobExecute::remove(std::vector<uint64_t> job_ids)
{
    return true;
}

void JobExecute::execute(JobBase *job) {
    job->execute();
}

void JobExecute::scan() {}

void JobExecute::main() {}
