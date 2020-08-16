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

void printDefaultConfig() {
    string yml = ansi(dir::app()+"config.yml");
    if(_access(yml.c_str(),0)!=0) {
        node config;
        config["enable"]="false";
        config["groups"]="0";
        for(int i=0; i<COMMAND_AMOUNT; i++) {
            config["command"][names[i]]["keyword"]=commands[i];
            config["command"][names[i]]["match"]=match_method[i];
            config["command"][names[i]]["priority"]=priority_requied[i];
        }
        ofstream osy(yml);
        osy<<config;
        osy.close();
    }
}

void printFiles() {
    printDefaultConfig();
}

void readCommands() {
    string yml = ansi(dir::app()+"config.yml");
    node config;

    try { //读取配置
        config = YAML::LoadFile(yml);
        int cnt=0;
        for(auto it:config["command"]) {
            int id=get_id_by_name(it.as<string>());
            if(id==-1)continue;
            commands[id]=it["keyword"].as<string>();
            match_method[id]=it["match"].as<int>();
            priority_requied[id]=it["priority"].as<int>();
        }
    } catch (ApiError &err) {
        logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
    }
}