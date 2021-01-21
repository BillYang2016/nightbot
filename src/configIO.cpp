#pragma once

#include "headers.h"
#include "configIO.hpp"

extern string names[COMMAND_AMOUNT+1];
extern vector<string> commands[COMMAND_AMOUNT+1];
extern string reply[COMMAND_AMOUNT+1];
extern int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
extern int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员

void commands_init() {
    names[0]="night",commands[0].push_back("晚安"),match_method[0]=0,priority_requied[0]=0,reply[0]="${at}晚安哦，你今天的清醒时间为${time_day}，是第${ranking}个睡觉的${call}~";
    names[1]="morning",commands[1].push_back("早安"),commands[1].push_back("早上好"),match_method[1]=0,priority_requied[1]=0,reply[1]="${at}早上好，你昨天的睡眠时间为${time_day}，是第${ranking}个起床的${call}~";
    names[2]="todaydata",commands[2].push_back("作息数据"),match_method[2]=0,priority_requied[2]=0,reply[2]="${at}今天有${number_asleep}个群友睡觉，${number_awake}个群友起床。";
    names[3]="mydata",commands[3].push_back("我的数据"),match_method[3]=0,priority_requied[3]=0,reply[3]="${at}你睡觉了${number_asleep}次，起床了${number_awake}次，平均在${avg_sleep}睡觉，平均在${avg_wake}起床。";
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
            config["command"][names[i]]["reply"]=reply[i];
        }
        config["night_without_morning"]="${at}晚安哦，您是今天第${ranking}个睡觉的群友~";
        config["morning_without_night"]="${at}早上好，您是今天第${ranking}个起床的群友~";
        config["multi"]["morning"]="${at}您今天已经早安过了哦~";
        config["multi"]["night"]="${at}您今天已经晚安过了哦~";
        config["out_of_time_period"]["morning"]="${at}您不在早安时间区间内哦，允许早安时间：${start_time}时~${end_time}时。";
        config["out_of_time_period"]["night"]="${at}您不在晚安时间区间内哦，允许晚安时间：${start_time}时~${end_time}时。";
        config["short_inteval"]["morning"]="${at}只睡这么会儿你不怕猝死？";
        config["short_inteval"]["night"]="${at}睡你麻痹起来嗨！";
        config["call"]["males"]="少年";
        config["call"]["females"]="少女";
        config["call"]["default"]="少女";
        config["time"]["start_hour"]=6;
        config["time"]["morning"]["accept_start_hour"]=0;
        config["time"]["morning"]["accept_end_hour"]=24;
        config["time"]["night"]["accept_start_hour"]=0;
        config["time"]["night"]["accept_end_hour"]=24;
        config["time"]["least_asleep"]=6;
        config["time"]["least_awake"]=6;
        config["time"]["cd"]["morning"]=12;
        config["time"]["cd"]["night"]=12;
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
            commands[i]=config["command"][names[i]]["keyword"].as<vector<string>>();
            match_method[i]=config["command"][names[i]]["match"].as<int>();
            priority_requied[i]=config["command"][names[i]]["priority"].as<int>();
            reply[i]=config["command"][names[i]]["reply"].as<string>();
        }
    } catch (ApiError &err) {
        logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
    }
}