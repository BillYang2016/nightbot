#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <direct.h>
#include <io.h>

#include <cqcppsdk/cqcppsdk.h>
#include <yaml-cpp/yaml.h>
#include "json.hpp"
#include "command.hpp"

#ifndef HEADERS

using json = nlohmann::json;
using node = YAML::Node;

using cq::utils::ansi;

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

#define HEADERS

#else

#define HEADERS extern

#endif

#include "stringextend.hpp"

bool Notify(const int &option,const GroupMessageEvent &event,const json &issue) {
    string yml = ansi(dir::app()+"config.yml");
    node config;

    int64_t admin=-1;
    try { //读取配置
        config = YAML::LoadFile(yml);
        admin=config["admin"].as<int64_t>();
    } catch (ApiError &err) {
        logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
    }

    if(option==0) { //新issue，发送给admin
        if(admin==-1)return false;
        try { //读取配置
            Message msg=config["notification"]["newissue"].as<string>();
            msg=replace_all_distinct(msg,"${id}",to_string(issue["id"].get<int>()));
            msg=replace_all_distinct(msg,"${title}",issue["title"].get<string>());
            msg=replace_all_distinct(msg,"${group}",to_string(issue["group"].get<int64_t>()));
            msg=replace_all_distinct(msg,"${content}",issue["content"].get<string>());
            GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),issue["author"].get<int64_t>());
            msg=replace_all_distinct(msg,"${author}",gm.card+"("+to_string(gm.user_id)+")");
            time_t t=issue["time"].get<time_t>();
            tm *tm;
            localtime_s(tm,&t);
            msg=replace_all_distinct(msg,"${time}",to_string(tm->tm_year+1900)+"年"+to_string(tm->tm_mon+1)+"月"+to_string(tm->tm_mday)+"日"+to_string(tm->tm_hour)+"时"+to_string(tm->tm_min)+"分"+to_string(tm->tm_sec)+"秒");
            send_private_message(admin,msg);
            return true;
        } catch (ApiError &err) {
            logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
            return false;
        }
    }

    return false;
}