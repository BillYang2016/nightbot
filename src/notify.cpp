#pragma once

#include "headers.h"

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
            msg=replace_all_distinct(msg,"${content}",issue["floor1"]["content"].get<string>());
            GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),issue["floor1"]["author"].get<int64_t>());
            msg=replace_all_distinct(msg,"${author}",gm.card+"("+to_string(gm.user_id)+")");
            time_t t=issue["floor1"]["time"].get<time_t>();
            struct tm tm;
            localtime_s(&tm,&t);
            msg=replace_all_distinct(msg,"${time}",to_string(tm.tm_year+1900)+"年"+to_string(tm.tm_mon+1)+"月"+to_string(tm.tm_mday)+"日"+to_string(tm.tm_hour)+"时"+to_string(tm.tm_min)+"分"+to_string(tm.tm_sec)+"秒");
            send_private_message(admin,msg);
            return true;
        } catch (ApiError &err) {
            logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
            return false;
        }
    } else if(option==1) { //关闭issue，发送给所有参与者
        try { //读取配置
            int floors=issue["floors"].get<int>();
            set<int64_t> participants;
            for(int i=1; i<=floors; i++) {
                int64_t id=issue["floor"+to_string(i)]["author"].get<int64_t>();
                if(participants.count(id)!=0||id==event.user_id)continue;
                participants.insert(id);
            }

            if(participants.count(admin)==0&&admin!=event.user_id)participants.insert(admin);

            Message msg=config["notification"]["closed"].as<string>();
            msg=replace_all_distinct(msg,"${id}",to_string(issue["id"].get<int>()));
            msg=replace_all_distinct(msg,"${title}",issue["title"].get<string>());
            msg=replace_all_distinct(msg,"${group}",to_string(issue["group"].get<int64_t>()));
            GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),event.user_id);
            msg=replace_all_distinct(msg,"${author}",gm.card+"("+to_string(gm.user_id)+")");
            time_t t=issue["floor"+to_string(floors)]["time"].get<time_t>();
            struct tm tm;
            localtime_s(&tm,&t);
            msg=replace_all_distinct(msg,"${time}",to_string(tm.tm_year+1900)+"年"+to_string(tm.tm_mon+1)+"月"+to_string(tm.tm_mday)+"日"+to_string(tm.tm_hour)+"时"+to_string(tm.tm_min)+"分"+to_string(tm.tm_sec)+"秒");
            
            for(auto id:participants) {
                send_private_message(id,msg); //因为Mirai的限制，私发消息需要是好友才能发送成功
            }
        } catch (ApiError &err) {
            logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
            return false;
        }
    } else if(option==2) { //issue被回复，发送给所有参与者
        try { //读取配置
            int floors=issue["floors"].get<int>();
            set<int64_t> participants;
            for(int i=1; i<=floors; i++) {
                int64_t id=issue["floor"+to_string(i)]["author"].get<int64_t>();
                if(participants.count(id)!=0||id==event.user_id)continue;
                participants.insert(id);
            }

            if(participants.count(admin)==0&&admin!=event.user_id)participants.insert(admin);

            Message msg=config["notification"]["reply"].as<string>();
            msg=replace_all_distinct(msg,"${id}",to_string(issue["id"].get<int>()));
            msg=replace_all_distinct(msg,"${title}",issue["title"].get<string>());
            msg=replace_all_distinct(msg,"${group}",to_string(issue["group"].get<int64_t>()));
            msg=replace_all_distinct(msg,"${content}",issue["floor"+to_string(floors)]["content"].get<string>());
            GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),event.user_id);
            msg=replace_all_distinct(msg,"${author}",gm.card+"("+to_string(gm.user_id)+")");
            time_t t=issue["floor"+to_string(floors)]["time"].get<time_t>();
            struct tm tm;
            localtime_s(&tm,&t);
            msg=replace_all_distinct(msg,"${time}",to_string(tm.tm_year+1900)+"年"+to_string(tm.tm_mon+1)+"月"+to_string(tm.tm_mday)+"日"+to_string(tm.tm_hour)+"时"+to_string(tm.tm_min)+"分"+to_string(tm.tm_sec)+"秒");
            
            for(auto id:participants) {
                send_private_message(id,msg); //因为Mirai的限制，私发消息需要是好友才能发送成功
            }
        } catch (ApiError &err) {
            logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
            return false;
        }
    }

    return false;
}