#pragma once

#include "headers.h"

#include "lcs.hpp"
#include "stringextend.hpp"

bool send_similar_issue(const GroupMessageEvent &event,int id,double score) {
    string yml = ansi(dir::app()+"config.yml");
    node config;

    vector<string> groups;
    try { //读取配置
        config = YAML::LoadFile(yml);
    } catch (ApiError &err) {
        logging::warning("加载数据","读取配置失败！错误码："+to_string(err.code));
        return false;
    }

    json data;
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
    } catch (ApiError &err) {
        logging::warning("加载数据","读取数据失败！错误码："+to_string(err.code));
        return false;
    }

    json issue=data["issue"+to_string(id)];

    Message tags;

    try {
        for(string tag:issue["tags"])tags+="["+tag+"] ";
    } catch (nlohmann::detail::type_error &err) { } //不存在，忽略

    Message msg=config["detect"]["reply"].as<string>();
    msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
    msg=replace_all_distinct(msg,"${title}",issue["title"].get<string>());
    msg=replace_all_distinct(msg,"${id}",to_string(issue["id"].get<int>()));
    msg=replace_all_distinct(msg,"${status}",issue["status"].get<string>());
    msg=replace_all_distinct(msg,"${score}",to_string(score)+"%");
    msg=replace_all_distinct(msg,"${tags}",tags);
    msg=replace_all_distinct(msg,"${content}",issue["floor1"]["content"].get<string>());
    GroupMember gm=get_group_member_info(issue["group"].get<int64_t>(),issue["floor1"]["author"].get<int64_t>());
    if(gm.card=="")msg=replace_all_distinct(msg,"${author}",gm.nickname+"("+to_string(gm.user_id)+")");
    else msg=replace_all_distinct(msg,"${author}",gm.card+"("+to_string(gm.user_id)+")");
    time_t t=issue["floor1"]["time"].get<time_t>();
    struct tm tm;
    localtime_s(&tm,&t);
    msg=replace_all_distinct(msg,"${time}",to_string(tm.tm_year+1900)+"年"+to_string(tm.tm_mon+1)+"月"+to_string(tm.tm_mday)+"日"+to_string(tm.tm_hour)+"时"+to_string(tm.tm_min)+"分"+to_string(tm.tm_sec)+"秒");

    send_group_message(event.group_id,msg);

    return true;
}

bool get_similar_issue(const GroupMessageEvent &event,int limit,bool search=0) {
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
        return false;
    }

    int number;
    try {
        number=data["ammount"].get<int>();
    } catch (nlohmann::detail::type_error &err) {
        return false; //无数据
    }

    double Maxscore=0;
    int id=0;
    bool asking=0;

    string msg=event.message;

    if(msg.find("http")!=string::npos || msg.find("https")!=string::npos)return false; //不检测链接

    if(msg.find("为什么")!=string::npos||msg.find("？")!=string::npos||msg.find("?")!=string::npos) {
        msg=replace_all_distinct(msg,"为什么","");
        msg=replace_all_distinct(msg,"？","");
        msg=replace_all_distinct(msg,"?","");
        asking=1;
    }

    json keywords;
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"keywords.json"));
        keywords=json::parse(jsonFile);
    } catch (ApiError &err) {
        logging::warning("加载数据","读取数据失败！错误码："+to_string(err.code));
        return false;
    }

    for(string x:keywords["keywords"])
        if(event.message.find(x)!=string::npos)msg=replace_all_distinct(msg,x,"");

    for(int i=1; i<=number; i++) {
        json issue=data["issue"+to_string(i)];
        string title=issue["title"];
        if(min(title.length(),msg.length())==0)continue;
        double titlescore=100.0*lcs(title,msg)/(min(title.length(),msg.length()));
        if(min(title.length(),msg.length())<=15)continue; //长度过小
        double score=titlescore;
        if(asking)score+=5;
        if(score>Maxscore) {
            Maxscore=score;
            id=i;
        }
    }

    if(search==1&&Maxscore<limit)return false;
    if(id==0)return false;

    logging::info("issuebot","已检测到相似issue，开始自动回复");

    return send_similar_issue(event,id,Maxscore);
}