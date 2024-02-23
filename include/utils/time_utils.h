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
#ifndef DNFSD_TIME_UTILS_H
#define DNFSD_TIME_UTILS_H

#include <ctime>
#include <cstdint>
#include <string>

class Time_T {
public:
    /*记录时间戳,用来同一个文件写入时排序*/
    time_t seconds;

    /*获取毫秒*/
    uint64_t millseconds;

    /*获取微秒*/
    uint64_t microseconds;
public:
    /*构造函数*/
    Time_T();

    /*将时间戳转化为日志
     * %a 星期几的简写
       %A 星期几的全称
       %b 月分的简写
       %B 月份的全称
       %c 标准的日期的时间串
       %C 年份的后两位数字
       %d 十进制表示的每月的第几天
       %D 月/天/年
       %e 在两字符域中，十进制表示的每月的第几天
       %F 年-月-日
       %g 年份的后两位数字，使用基于周的年
       %G 年分，使用基于周的年
       %h 简写的月份名
       %H 24小时制的小时
       %I 12小时制的小时
       %j 十进制表示的每年的第几天
       %m 十进制表示的月份
       %M 十时制表示的分钟数
       %n 新行符
       %p 本地的AM或PM的等价显示
       %r 12小时的时间
       %R 显示小时和分钟：hh:mm
       %S 十进制的秒数
       %t 水平制表符
       %T 显示时分秒：hh:mm:ss
       %u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
       %U 第年的第几周，把星期日做为第一天（值从0到53）
       %V 每年的第几周，使用基于周的年
       %w 十进制表示的星期几（值从0到6，星期天为0）
       %W 每年的第几周，把星期一做为第一天（值从0到53）
       %x 标准的日期串
       %X 标准的时间串
       %y 不带世纪的十进制年份（值从0到99）
       %Y 带世纪部分的十进制年份
       %z，%Z 时区名称，如果不能得到时区名称则返回空字符。
       %% 百分号
     * params format:转化的日期格式
     * return: 转化完成的日期字符串
     * */
    std::string format(const std::string &format);

    /*重载大于运算符*/
    bool operator>(Time_T &s) const;

    /*重载小于运算符*/
    bool operator<(Time_T &s) const;
};


#endif //DNFSD_TIME_UTILS_H
