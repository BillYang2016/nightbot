#pragma once

#include "headers.h"

#include "stringextend.hpp"
#include "configIO.hpp"
#include "response.hpp"

extern string names[COMMAND_AMOUNT+1];
extern vector<string> commands[COMMAND_AMOUNT+1];
extern string reply[COMMAND_AMOUNT+1];
extern int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
extern int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员

CQ_INIT {
    on_enable([] { 
        logging::info("启用", "nightbot已启用");
        commands_init();
        printFiles();
        readCommands();
    });

    on_group_message([](const GroupMessageEvent &event) {
        string yml = ansi(dir::app()+"config.yml");
        node config;

        vector<string> groups;
        try { //读取配置
            config = YAML::LoadFile(yml);
            string string_groups=config["groups"].as<string>();
            groups=stringSplit(string_groups,",");
        } catch (ApiError &err) {
            logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
        }

        if(config["enable"].as<string>()!="true")return; //插件未启用

        if(!(groups.size()==1&&groups.back()=="1")) { //groups填写1全体启用（issue#1.1）
            bool ENABLED=0;
            for(auto group:groups)if(group==to_string(event.group_id)) {ENABLED=1;break;}
            if (ENABLED == 0) return; // 不在启用的群中, 忽略
        }

        int commandid=-1;

        for(int i=0; i<COMMAND_AMOUNT; i++) { //判断触发命令
            int meth=match_method[i];
            int prio=priority_requied[i];
            bool flag=0; //是否匹配

            for(string command:commands[i]) {
                if(meth==0)flag=command==event.message;
                else flag=event.message.find(command)!=string::npos;
                if(flag)break;
            }

            if(flag) {
                commandid=i;
                break;
            }
        }

        if(~commandid) {
            int prio=priority_requied[commandid];
            GroupRole role=get_group_member_info(event.group_id,event.user_id).role;
            if(role==GroupRole::MEMBER&&prio==2)send_group_message(event.group_id,MessageSegment::at(event.user_id)+"你的权限不足！"); //初步判断权限等级
            else Response(commandid,event);
            return;
        }

    });
}
