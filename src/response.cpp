#pragma once

#include "headers.h"

#include "stringextend.hpp"
#include "notify.hpp"

extern string names[COMMAND_AMOUNT+1];
extern string commands[COMMAND_AMOUNT+1];
extern int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
extern int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员

int get_id_by_title(const json &data,const string &title) {
    int number=data["ammount"].get<int>();
    for(int i=1; i<=number; i++)if(to_string(i)==title)return i; //使用id，直接返回
    int Min=INT_MAX,id=-1;
    for(int i=1; i<=number; i++) {
        string issuetitle=data["issue"+to_string(i)]["title"].get<string>();
        if(issuetitle.find(title)!=string::npos&&issuetitle.length()<Min) {
            Min=issuetitle.length();
            id=i;
        }
    }
    return id;
}

bool Response(const int &id,const GroupMessageEvent &event) {
    json data;
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
    } catch (ApiError &err) {
        logging::warning("加载数据","读取数据失败！错误码："+to_string(err.code));
        return false;
    } catch (nlohmann::detail::parse_error &err) { //json不存在
        logging::info("加载数据","json数据不存在，重新创建");
        _mkdir(ansi(dir::app()+"groups\\").c_str());
        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
        os << data.dump(4) << endl;
        os.close();
    }

    string msg=event.message;
    char newl[2]= {char(13),char(10)}; //处理一下换行
    msg=replace_all_distinct(msg,newl,"[换行符]");
    msg=replace_all_distinct(msg,string(1,char(13)),"[换行符]");
    msg=replace_all_distinct(msg,string(1,char(10)),"[换行符]");
    msg=replace_all_distinct(msg,string(1,'\n'),"[换行符]");

    if(id==0) { //添加issue
        int number;
        try {
            number=data["ammount"].get<int>()+1;
        } catch (nlohmann::detail::type_error &err) {
            number=1;
        }
        data["ammount"]=number;
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[0]+"#\"issue名\"#\"内容\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        cmd[2]=replace_all_distinct(cmd[2],"[cap3]","#"); //转义
        data["issue"+to_string(number)]["title"]=cmd[1];
        data["issue"+to_string(number)]["id"]=number;
        data["issue"+to_string(number)]["status"]="Open";
        data["issue"+to_string(number)]["group"]=event.group_id;
        data["issue"+to_string(number)]["floors"]=1;
        data["issue"+to_string(number)]["floor1"]["content"]=cmd[2];
        data["issue"+to_string(number)]["floor1"]["author"]=event.user_id;
        data["issue"+to_string(number)]["floor1"]["time"]=time(NULL);

        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        os << data.dump(4) << endl;
        os.close();

        send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已成功创建新issue，序号为"+to_string(number)+"。");

        Notify(0,event,data["issue"+to_string(number)]); //推送消息
        return true;
    } else if(id==1) { //关闭issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=2) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[1]+"#\"issue名\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue");
            return false;
        }

        json issue=data["issue"+to_string(id)];
        issue["status"]="Closed";
        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="Closed this issue";
        issue["floor"+to_string(floors)]["author"]=event.user_id;
        issue["floor"+to_string(floors)]["time"]=time(NULL);

        GroupRole role=get_group_member_info(event.group_id,event.user_id).role;
        try {
            if(role==GroupRole::MEMBER && event.user_id!=issue["floor1"]["author"].get<int64_t>()) { //权限不足
                send_group_message(event.group_id,MessageSegment::at(event.user_id)+"你的权限不足！");
                return false;
            }

            data["issue"+to_string(id)]=issue;

            ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            os << data.dump(4) << endl;
            os.close();

            Notify(1,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("关闭issue","json储存数据不全，指令响应失败");
            return false;
        }
    }

    return false;
}