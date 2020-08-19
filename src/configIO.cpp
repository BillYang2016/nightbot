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
	names[7]="assign",commands[7]="指派",match_method[7]=1,priority_requied[7]=2;
	names[8]="view",commands[8]="查看issue",match_method[8]=1,priority_requied[8]=0;
	names[9]="deltag",commands[9]="删除tag",match_method[9]=1,priority_requied[9]=1;
	names[10]="unassign",commands[10]="取消指派",match_method[10]=1,priority_requied[10]=2;
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
        config["friendrequest"]="0";
        for(int i=0; i<COMMAND_AMOUNT; i++) {
            config["command"][names[i]]["keyword"]=commands[i];
            config["command"][names[i]]["match"]=match_method[i];
            config["command"][names[i]]["priority"]=priority_requied[i];
        }
        config["notification"]["newissue"]="您在${group}群有新的issue啦！\n${title} (#${id})\n${content}\n时间：${time}\n发起者：${author}";
        config["notification"]["closed"]="${group}群的issue被${author}关闭啦！\n${title} (#${id})\n时间：${time}";
        config["notification"]["reply"]="${group}群的issue被${author}回复啦！\n${title} (#${id})\n${content}\n时间：${time}";
        config["notification"]["reopened"]="${group}群的issue被${author}重新开启啦！\n${title} (#${id})\n时间：${time}";
        config["notification"]["newtag"]="${author}为${group}群的issue添加了标签！\n${title} (#${id})\n${content}\n时间：${time}";
        config["notification"]["deltag"]="${author}为${group}群的issue删除了标签！\n${title} (#${id})\n${content}\n时间：${time}";
        config["notification"]["assign"]="${author}为${group}群的issue添加了指派！\n${title} (#${id})\n${content}\n时间：${time}";
        config["notification"]["unassign"]="${author}为${group}群的issue取消了指派！\n${title} (#${id})\n${content}\n时间：${time}";
        config["reply"]["view"]="${at}\n${title} (#${id}) [${status}] (${pagenum}页)\nTags: ${tags}\nAssignees: ${assignees}\n----------\n${vieweach}";
        config["reply"]["search"]="${at}\n找到以下issue：(${pagenum}页)\n${searcheach}";
        config["reply"]["vieweach"]="${content}\n作者：${author}\n时间：${time}\n----------";
        config["reply"]["searcheach"]="- ${title} (#${id}) [${status}] ${tags}";
        config["reply"]["view_perpage"]=3;
        config["reply"]["search_perpage"]=5;
        config["detect"]["enable"]="false";
        config["detect"]["limit"]=60;
        config["detect"]["reply"]="${at}为您找到最为匹配的issue，匹配度${score}：\n${title} (#${id}) [${status}]\n${content}\n请输入\"查看issue#${id}\"以获取详细信息！";
        ofstream osy(yml);
        osy<<config;
        osy.close();
    }
}

void printDefaultKeywords() {
    string json = ansi(dir::app()+"keywords.json");
    if(_access(json.c_str(),0)!=0) {
        json data;
        
        data["keywords"].push_back("请问");
        data["keywords"].push_back("问一下");
        data["keywords"].push_back("提问");
        data["keywords"].push_back("怎么办");
        data["keywords"].push_back("怎么用");
        data["keywords"].push_back("怎么弄");
        data["keywords"].push_back("怎么搞");

        ofstream os(ansi(dir::app()+"keywords.json"));
        os << data.dump(4) << endl;
        os.close();
    }
}

void printFiles() {
    printDefaultConfig();
    printDefaultKeywords();
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