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

extern int64_t get_number_by_string(const string &s); //提取字符串中的数字

bool Response(const int &eventtype,const GroupMessageEvent &event) {
    string yml = ansi(dir::app()+"config.yml");
    node config;

    try { //读取配置
        config = YAML::LoadFile(yml);
    } catch (ApiError &err) {
        logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
    }

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
    msg=replace_all_distinct(msg,newl,"\n");
    msg=replace_all_distinct(msg,string(1,char(13)),"\n");
    msg=replace_all_distinct(msg,string(1,char(10)),"\n");

    if(eventtype==0) { //添加issue
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
    } else if(eventtype==1) { //关闭issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=2) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[1]+"#\"issue名或id\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }

        json issue=data["issue"+to_string(id)];

        if(issue["status"]=="Closed") {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"该issue已被关闭！");
            return false;
        }

        issue["status"]="Closed";
        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="*Closed this issue";
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

            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已关闭“"+issue["title"].get<string>()+"” (#"+to_string(id)+")。");

            Notify(1,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("关闭issue","json储存数据不全，指令响应失败");
            return false;
        }
    } else if(eventtype==2) { //回复issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[2]+"#\"issue名或id\"#\"内容\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        cmd[2]=replace_all_distinct(cmd[2],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }
        
        json issue=data["issue"+to_string(id)];

        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]=cmd[2];
        issue["floor"+to_string(floors)]["author"]=event.user_id;
        issue["floor"+to_string(floors)]["time"]=time(NULL);
        
        data["issue"+to_string(id)]=issue;
        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        os << data.dump(4) << endl;
        os.close();

        send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已成功回复“"+issue["title"].get<string>()+"” (#"+to_string(id)+")。");

        Notify(2,event,issue); //推送消息
    } else if(eventtype==3) { //搜索issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=2&&cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[3]+"#\"关键词、tag或status\"#页码（选填）");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义

        int page=0;
        if(cmd.size()==3)page=atoi(cmd[2].c_str());
        if(page==0)page=1;

        int number;
        try {
            number=data["ammount"].get<int>();
        } catch (nlohmann::detail::type_error &err) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"暂无issue！");
            return false;
        }

        vector<string> issues;
        transform(cmd[1].begin(),cmd[1].end(),cmd[1].begin(),::tolower);

        for(int i=1; i<=number; i++) {
            json issue=data["issue"+to_string(i)];
            string title=issue["title"].get<string>(),status=issue["status"].get<string>();
            transform(title.begin(),title.end(),title.begin(),::tolower);
            transform(status.begin(),status.end(),status.begin(),::tolower);

            bool find=0; //包含tag
            json tags=issue["tags"];

            for(string tag:issue["tags"]) {
                transform(tag.begin(),tag.end(),tag.begin(),::tolower);
                if(tag==cmd[1]) {find=1;break;}
            }

            if(find || title.find(cmd[1])!=string::npos || status.find(cmd[1])!=string::npos) { //包含tag或者包含标题或者包含状态
                Message msg=config["reply"]["searcheach"].as<string>();
                msg=replace_all_distinct(msg,"${title}",issue["title"].get<string>());
                msg=replace_all_distinct(msg,"${id}",to_string(issue["id"].get<int>()));
                msg=replace_all_distinct(msg,"${status}",issue["status"].get<string>());
                string tags;
                for(string tag:issue["tags"])tags+="["+tag+"] ";
                msg=replace_all_distinct(msg,"${tags}",tags);
                issues.push_back(msg);
            }
        }

        if(issues.empty()) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"无合法issue！");
            return false;
        }

        int perpage=config["reply"]["search_perpage"].as<int>();
        if(perpage==0)perpage=5;

        int pages=(issues.size()-1)/perpage+1;
        if(page<1||page>pages) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该页码（"+to_string(page)+"/"+to_string(pages)+"）！");
            return false;
        }

        Message reply=config["reply"]["search"].as<string>(),searcheach;

        reply=replace_all_distinct(reply,"${at}",MessageSegment::at(event.user_id));
        reply=replace_all_distinct(reply,"${pagenum}",to_string(page)+"/"+to_string(pages));

        for(int i=0; i<perpage; i++) {
            int num=(page-1)*perpage+i;
            if(num>=issues.size())break;
            searcheach+=issues[num]+"\n";
        }

        reply=replace_all_distinct(reply,"${searcheach}",searcheach);
        
        send_group_message(event.group_id,reply);
    } else if(eventtype==4) { //重开issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=2) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[4]+"#\"issue名或id\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }

        json issue=data["issue"+to_string(id)];

        if(issue["status"]=="Open") {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"该issue Open中！");
            return false;
        }

        issue["status"]="Open";
        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="*Reopened this issue";
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

            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已重新开启“"+issue["title"].get<string>()+"” (#"+to_string(id)+")。");

            Notify(4,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("重开issue","json储存数据不全，指令响应失败");
            return false;
        }
    } else if(eventtype==5) { //搜索全部issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=1&&cmd.size()!=2) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[5]+"#页码（选填）");
            return false;
        }

        int page=0;
        if(cmd.size()==2)page=atoi(cmd[1].c_str());
        if(page==0)page=1;

        int number;
        try {
            number=data["ammount"].get<int>();
        } catch (nlohmann::detail::type_error &err) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"暂无issue！");
            return false;
        }

        int perpage=config["reply"]["search_perpage"].as<int>();
        if(perpage==0)perpage=5;

        int pages=(number-1)/perpage+1;
        if(page<1||page>pages) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该页码（"+to_string(page)+"/"+to_string(pages)+"）！");
            return false;
        }

        Message reply=config["reply"]["search"].as<string>(),searcheach;

        reply=replace_all_distinct(reply,"${at}",MessageSegment::at(event.user_id));
        reply=replace_all_distinct(reply,"${pagenum}",to_string(page)+"/"+to_string(pages));

        for(int i=1; i<=perpage; i++) {
            int num=(page-1)*perpage+i;
            if(num>=number)break;
            json issue=data["issue"+to_string(num)];
            Message msg=config["reply"]["searcheach"].as<string>();
            msg=replace_all_distinct(msg,"${title}",issue["title"].get<string>());
            msg=replace_all_distinct(msg,"${id}",to_string(issue["id"].get<int>()));
            msg=replace_all_distinct(msg,"${status}",issue["status"].get<string>());
            string tags;
            for(string tag:issue["tags"])tags+="["+tag+"] ";
            msg=replace_all_distinct(msg,"${tags}",tags);
            searcheach+=msg+"\n";
        }

        reply=replace_all_distinct(reply,"${searcheach}",searcheach);
        
        send_group_message(event.group_id,reply);
    } else if(eventtype==6) { //添加tag
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[6]+"#\"issue名或id\"#\"tag名\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        cmd[2]=replace_all_distinct(cmd[2],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }

        json issue=data["issue"+to_string(id)];

        for(auto it=issue["tags"].begin(); it!=issue["tags"].end(); it++)if(*it==cmd[2]) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"该tag已存在！");
            return false;
        }

        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="*Added tag \""+cmd[2]+"\"";
        issue["tags"].push_back(cmd[2]);
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

            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已为“"+issue["title"].get<string>()+"” (#"+to_string(id)+") 添加标签“"+cmd[2]+"”。");

            Notify(6,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("添加tag","json储存数据不全，指令响应失败");
            return false;
        }
    } else if(eventtype==7) { //添加指派
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[7]+"#\"issue名或id\"#\"@对象或输入qq号\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        cmd[2]=replace_all_distinct(cmd[2],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }

        int64_t qqid=get_number_by_string(cmd[2]);

        json issue=data["issue"+to_string(id)];

        for(auto it=issue["assignees"].begin(); it!=issue["assignees"].end(); it++)if(*it==qqid) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"该assignee已存在！");
            return false;
        }

        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="*Assigned \""+cmd[2]+"\"";
        issue["assignees"].push_back(qqid);
        issue["floor"+to_string(floors)]["author"]=event.user_id;
        issue["floor"+to_string(floors)]["time"]=time(NULL);

        GroupRole role=get_group_member_info(event.group_id,event.user_id).role;
        try {
            data["issue"+to_string(id)]=issue;

            ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            os << data.dump(4) << endl;
            os.close();

            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已为“"+issue["title"].get<string>()+"” (#"+to_string(id)+") 添加指派“"+MessageSegment::at(qqid)+"”。");

            Notify(7,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("assign","json储存数据不全，指令响应失败");
            return false;
        }
    } else if(eventtype==8) { //查看issue
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=2&&cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[8]+"#\"issue名或id\"#\"页码（选填）\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义

        int page=0;
        if(cmd.size()==3)page=atoi(cmd[2].c_str());
        if(page==0)page=1;

        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }
        
        json issue=data["issue"+to_string(id)];

        int perpage=config["reply"]["view_perpage"].as<int>();
        if(perpage==0)perpage=5;

        int floors=issue["floors"].get<int>();
        int pages=(floors-1)/perpage+1;

        if(page<1||page>pages) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该页码（"+to_string(page)+"/"+to_string(pages)+"）！");
            return false;
        }

        Message reply=config["reply"]["view"].as<string>(),vieweach;

        reply=replace_all_distinct(reply,"${at}",MessageSegment::at(event.user_id));
        reply=replace_all_distinct(reply,"${title}",issue["title"].get<string>());
        reply=replace_all_distinct(reply,"${id}",to_string(id));
        reply=replace_all_distinct(reply,"${status}",issue["status"].get<string>());
        reply=replace_all_distinct(reply,"${pagenum}",to_string(page)+"/"+to_string(pages));

        string tags,assignees;
        for(string tag:issue["tags"])tags+="["+tag+"] ";

        reply=replace_all_distinct(reply,"${tags}",tags);

        for(int64_t qqid:issue["assignees"]) {
            GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),qqid);
            if(gm.card=="")assignees+=gm.nickname+"("+to_string(gm.user_id)+") ";
            else assignees+=gm.card+"("+to_string(gm.user_id)+") ";
        }

        reply=replace_all_distinct(reply,"${assignees}",assignees);

        for(int i=1; i<=perpage; i++) {
            int num=(page-1)*perpage+i;
            if(num>floors)break;
            Message msg=config["reply"]["vieweach"].as<string>();
            msg=replace_all_distinct(msg,"${content}",issue["floor"+to_string(num)]["content"]);
            GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),issue["floor"+to_string(num)]["author"].get<int64_t>());
            if(gm.card=="")msg=replace_all_distinct(msg,"${author}",gm.nickname+"("+to_string(gm.user_id)+")");
            else msg=replace_all_distinct(msg,"${author}",gm.card+"("+to_string(gm.user_id)+")");
            time_t t=issue["floor"+to_string(num)]["time"].get<time_t>();
            struct tm tm;
            localtime_s(&tm,&t);
            msg=replace_all_distinct(msg,"${time}",to_string(tm.tm_year+1900)+"年"+to_string(tm.tm_mon+1)+"月"+to_string(tm.tm_mday)+"日"+to_string(tm.tm_hour)+"时"+to_string(tm.tm_min)+"分"+to_string(tm.tm_sec)+"秒");
            vieweach+=msg+"\n";
        }

        reply=replace_all_distinct(reply,"${vieweach}",vieweach);

        send_group_message(event.group_id,reply);
    } else if(eventtype==9) { //删除tag
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[9]+"#\"issue名或id\"#\"tag名\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        cmd[2]=replace_all_distinct(cmd[2],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }

        json issue=data["issue"+to_string(id)];

        bool flag=0;
        for(auto it=issue["tags"].begin(); it!=issue["tags"].end(); it++)if(*it==cmd[2]) {
            flag=1;
            issue["tags"].erase(it);
            break;
        }

        if(flag==0) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该tag！");
            return false;
        }

        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="*Removed tag \""+cmd[2]+"\"";
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

            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已为“"+issue["title"].get<string>()+"” (#"+to_string(id)+") 删除标签“"+cmd[2]+"”。");

            Notify(9,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("删除tag","json储存数据不全，指令响应失败");
            return false;
        }
    } else if(eventtype==10) { //取消指派
        msg=replace_all_distinct(msg,"\\#","[cap3]"); //转义
        vector<string> cmd=stringSplit(msg,"#");
        if(cmd.size()!=3) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"格式错误，正确格式为"+commands[10]+"#\"issue名或id\"#\"@对象或输入qq号\"");
            return false;
        }
        cmd[1]=replace_all_distinct(cmd[1],"[cap3]","#"); //转义
        cmd[2]=replace_all_distinct(cmd[2],"[cap3]","#"); //转义
        int id=get_id_by_title(data,cmd[1]);
        if(id==-1) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该issue！");
            return false;
        }

        int64_t qqid=get_number_by_string(cmd[2]);

        json issue=data["issue"+to_string(id)];

        bool flag=0;
        for(auto it=issue["assignees"].begin(); it!=issue["assignees"].end(); it++)if(*it==qqid) {
            flag=1;
            issue["assignees"].erase(it);
            break;
        }

        if(flag==0) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"不存在该成员！");
            return false;
        }

        int floors=issue["floors"].get<int>()+1;
        issue["floors"]=floors;
        issue["floor"+to_string(floors)]["content"]="*Unassigned \""+cmd[2]+"\"";
        issue["floor"+to_string(floors)]["author"]=event.user_id;
        issue["floor"+to_string(floors)]["time"]=time(NULL);

        GroupRole role=get_group_member_info(event.group_id,event.user_id).role;
        try {
            data["issue"+to_string(id)]=issue;

            ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            os << data.dump(4) << endl;
            os.close();

            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已为“"+issue["title"].get<string>()+"” (#"+to_string(id)+") 取消指派“"+MessageSegment::at(qqid)+"”。");

            Notify(10,event,issue); //推送消息
        } catch (nlohmann::detail::type_error &err) { //不存在
            logging::warning("assign","json储存数据不全，指令响应失败");
            return false;
        }
    }

    return false;
}