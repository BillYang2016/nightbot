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
#include "configIO.hpp"
#include "response.hpp"

CQ_INIT {
    on_enable([] { 
        logging::info("启用", "issuebot已启用");
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

        for(int i=0; i<COMMAND_AMOUNT; i++) { //判断触发命令
            int meth=match_method[i];
            string command=commands[i];
            int prio=priority_requied[i];
            bool flag=0; //是否匹配

            if(meth==0)flag=command==event.message;
            else flag=event.message.find(command)!=string::npos;
            if(!flag)continue;

            GroupRole role=get_group_member_info(event.group_id,event.user_id).role;
            if(role==GroupRole::MEMBER&&prio==2)send_group_message(event.group_id,MessageSegment::at(event.user_id)+"你的权限不足！"); //初步判断权限等级
            else Response(i,event);
            return;
        }

    });
}
