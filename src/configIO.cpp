#pragma once

#include "headers.h"

extern string names[COMMAND_AMOUNT+1];
extern string commands[COMMAND_AMOUNT+1];
extern int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
extern int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员

void commands_init() {
	names[0]="add",commands[0]="添加issue",match_method[0]=1,priority_requied[0]=0;
	names[1]="close",commands[1]="关闭issue",match_method[1]=1,priority_requied[1]=1;
	names[2]="reply",commands[2]="回复issue",match_method[2]=1,priority_requied[2]=0;
	names[3]="search",commands[3]="搜索issue",match_method[3]=1,priority_requied[3]=0;
	names[4]="reopen",commands[4]="重开issue",match_method[4]=1,priority_requied[4]=1;
	names[5]="all",commands[5]="全部issue",match_method[5]=0,priority_requied[5]=0;
	names[6]="newtag",commands[6]="添加tag",match_method[6]=1,priority_requied[6]=1;
	names[7]="assign",commands[7]="指派issue",match_method[7]=1,priority_requied[7]=2;
}

int get_id_by_name(string name) {
	for(int i=0; i<COMMAND_AMOUNT; i++)if(name==names[i])return i;
	return -1;
}

void printDefaultConfig() {
    string yml = ansi(dir::app()+"config.yml");
    if(_access(yml.c_str(),0)!=0) {
        node config;
        config["enable"]="false";
        config["groups"]="0";
        config["admin"]="-1";
        for(int i=0; i<COMMAND_AMOUNT; i++) {
            config["command"][names[i]]["keyword"]=commands[i];
            config["command"][names[i]]["match"]=match_method[i];
            config["command"][names[i]]["priority"]=priority_requied[i];
        }
        config["notification"]["newissue"]="您在${group}群有新的issue啦！\n${title} (#${id})\n时间：${time}\n发起者：${author}";
        config["notification"]["closed"]="${group}群的issue被${author}关闭啦！\n${title} (#${id})\n时间：${time}";
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
        for(int i=0; i<COMMAND_AMOUNT; i++) {
            commands[i]=config["command"][names[i]]["keyword"].as<string>();
            match_method[i]=config["command"][names[i]]["match"].as<int>();
            priority_requied[i]=config["command"][names[i]]["priority"].as<int>();
        }
    } catch (ApiError &err) {
        logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
    }
}