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
#include "notify.hpp"

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
        data["issue"+to_string(number)]["title"]=cmd[1];
        data["issue"+to_string(number)]["id"]=to_string(number);
        data["issue"+to_string(number)]["floors"]=1;
        data["issue"+to_string(number)]["floor1"]["content"]=cmd[2];
        data["issue"+to_string(number)]["floor1"]["group"]=event.group_id;
        data["issue"+to_string(number)]["floor1"]["author"]=event.user_id;
        data["issue"+to_string(number)]["floor1"]["time"]=time(NULL);
        send_group_message(event.group_id,MessageSegment::at(event.user_id)+"已成功创建新issue，序号为"+to_string(number)+"。");

        Notify(0,event,data["issue"+to_string(number)]); //推送消息
        return true;
    }
    return false;
}