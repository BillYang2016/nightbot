#pragma once

#include "headers.h"

#include "stringextend.hpp"

extern string names[COMMAND_AMOUNT+1];
extern vector<string> commands[COMMAND_AMOUNT+1];
extern string reply[COMMAND_AMOUNT+1];
extern int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
extern int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员

int night_ranking=0,day_ranking=0;

extern int64_t get_number_by_string(const string &s); //提取字符串中的数字

bool get_ranking(json &data,const GroupMessageEvent &event) {
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        jsonFile.close();
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

    try { //读取早安排名
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        day_ranking=data["day_ranking"].get<int>();
        jsonFile.close();
    } catch (ApiError &err) {
        logging::warning("加载数据","读取排名数据失败！错误码："+to_string(err.code));
        return false;
    } catch (nlohmann::detail::parse_error &err) { //json不存在
        logging::info("加载数据","json数据不存在，重新创建");
        _mkdir(ansi(dir::app()+"groups\\").c_str());
        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
        os << data.dump(4) << endl;
        os.close();
        day_ranking=0;
    } catch (nlohmann::detail::type_error &err) { //json没有ranking数据
        logging::info("加载数据","json数据不存在");
        day_ranking=0;
    }

    try { //读取晚安排名
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        night_ranking=data["night_ranking"].get<int>();
        jsonFile.close();
    } catch (ApiError &err) {
        logging::warning("加载数据","读取排名数据失败！错误码："+to_string(err.code));
        return false;
    } catch (nlohmann::detail::parse_error &err) { //json不存在
        logging::info("加载数据","json数据不存在，重新创建");
        _mkdir(ansi(dir::app()+"groups\\").c_str());
        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
        os << data.dump(4) << endl;
        os.close();
        night_ranking=0;
    } catch (nlohmann::detail::type_error &err) { //json没有ranking数据
        logging::info("加载数据","json数据不存在");
        night_ranking=0;
    }

    return true;
}

bool clear_ranking(json &data,const GroupMessageEvent &event) {
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        jsonFile.close();
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

    ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));

    data["day_ranking"]=data["night_ranking"]=day_ranking=night_ranking=0;

    os << data.dump(4) << endl;
    os.close();

    return true;
}

bool increase_ranking(json &data,const int &type,const GroupMessageEvent &event) {
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        jsonFile.close();
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
    
    ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));

    if(type==0)data["night_ranking"]=++night_ranking;
    else data["day_ranking"]=++day_ranking;

    os << data.dump(4) << endl;
    os.close();

    return true;
}

bool get_data(const int &type,int &year,int &month,int &day,time_t &lastt,const GroupMessageEvent &event) {
    json data;
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        jsonFile.close();
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
    
    struct tm t;
    if(type==0) { //晚安
        try { //读取晚安数据
            ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            data=json::parse(jsonFile);
            jsonFile.close();
            json user=data[to_string(event.user_id)];
            lastt=user["night_lasttime"].get<time_t>();
            localtime_s(&t,&lastt);
            year=t.tm_year,month=t.tm_mon,day=t.tm_mday;
        } catch (ApiError &err) {
            logging::warning("加载数据","读取签到数据失败！错误码："+to_string(err.code));
            return false;
        } catch (nlohmann::detail::parse_error &err) { //json不存在
            logging::info("加载数据","json数据不存在，重新创建");
            _mkdir(ansi(dir::app()+"groups\\").c_str());
            ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
            os << data.dump(4) << endl;
            os.close();
            lastt=1000000000;
            localtime_s(&t,&lastt);
            year=t.tm_year,month=t.tm_mon,day=t.tm_mday;
        } catch (nlohmann::detail::type_error &err) { //json没有用户晚安数据
            logging::info("加载数据","json数据不存在");
            lastt=1000000000;
            localtime_s(&t,&lastt);
            year=t.tm_year,month=t.tm_mon,day=t.tm_mday;
        }
    } else { //早安
        try { //读取早安数据
            ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            data=json::parse(jsonFile);
            jsonFile.close();
            json user=data[to_string(event.user_id)];
            lastt=user["day_lasttime"].get<time_t>();
            localtime_s(&t,&lastt);
            year=t.tm_year,month=t.tm_mon,day=t.tm_mday;
        } catch (ApiError &err) {
            logging::warning("加载数据","读取签到数据失败！错误码："+to_string(err.code));
            return false;
        } catch (nlohmann::detail::parse_error &err) { //json不存在
            logging::info("加载数据","json数据不存在，重新创建");
            _mkdir(ansi(dir::app()+"groups\\").c_str());
            ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
            data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
            os << data.dump(4) << endl;
            os.close();
            lastt=1000000000;
            localtime_s(&t,&lastt);
            year=t.tm_year,month=t.tm_mon,day=t.tm_mday;
        } catch (nlohmann::detail::type_error &err) { //json没有用户早安数据
            logging::info("加载数据","json数据不存在");
            lastt=1000000000;
            localtime_s(&t,&lastt);
            year=t.tm_year,month=t.tm_mon,day=t.tm_mday;
        }
    }

    return true;
}

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
        jsonFile.close();
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

    get_ranking(data,event); //缓存同步

    int start_hour=config["time"]["start_hour"].as<int>();

    struct tm t;
    time_t night_lastt,day_lastt;
    int lastyear,nowyear,lastmonth,nowmonth,lastday,nowday;
    
    time_t nowt=time(NULL);
    localtime_s(&t,&nowt);
    nowyear=t.tm_year,nowmonth=t.tm_mon,nowday=t.tm_mday;

    int running_day=0;

    try { //读取running_day数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data=json::parse(jsonFile);
        jsonFile.close();
        running_day=data["running_day"].get<int>();
    } catch (ApiError &err) {
        logging::warning("加载数据","读取签到数据失败！错误码："+to_string(err.code));
        return false;
    } catch (nlohmann::detail::parse_error &err) { //json不存在
        logging::info("加载数据","json数据不存在，重新创建");
        _mkdir(ansi(dir::app()+"groups\\").c_str());
        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
        os << data.dump(4) << endl;
        os.close();
        running_day=0;
    } catch (nlohmann::detail::type_error &err) { //json没有用户早安数据
        logging::info("加载数据","json数据不存在");
        running_day=0;
    }

    if(nowday!=running_day&&t.tm_hour>=start_hour) {
        data["running_day"]=running_day=nowday;
        ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
        os << data.dump(4) << endl;
        os.close();
        clear_ranking(data,event);
    }

    get_ranking(data,event); //再一次缓存同步

    if(eventtype==0) { //晚安
        get_data(0,lastyear,lastmonth,lastday,night_lastt,event);

        int accept_start_hour=config["time"]["night"]["accept_start_hour"].as<int>(),accept_end_hour=config["time"]["night"]["accept_end_hour"].as<int>();

        if((accept_start_hour<accept_end_hour&&(t.tm_hour<accept_start_hour||t.tm_hour>=accept_end_hour)) || (accept_start_hour>=accept_end_hour&&(t.tm_hour<accept_start_hour&&t.tm_hour>=accept_end_hour))) { //不在晚安区间
            Message msg=config["out_of_time_period"]["night"].as<string>();
            if(msg=="null")return true;
            msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
            msg=replace_all_distinct(msg,"${start_time}",to_string(config["time"]["night"]["accept_start_hour"].as<int>()));
            msg=replace_all_distinct(msg,"${start_time}",to_string(config["time"]["night"]["accept_end_hour"].as<int>()));
            send_group_message(event.group_id,msg);
            return true;
        }

        try {
            if(lastyear==nowyear && lastmonth==nowmonth && lastday==nowday) { //今天已经晚安过了
                Message msg=config["multi"]["night"].as<string>();
                logging::info("晚安",to_string(event.user_id)+"重复晚安");
                if(msg=="null")return true;
                msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
                send_group_message(event.group_id,msg);
            } else {
                increase_ranking(data,0,event);

                ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
                json user=data[to_string(event.user_id)];
                user["night_lasttime"]=time(NULL);
                try {
                    user["night_counts"]=user["night_counts"].get<int>()+1;
                } catch (nlohmann::detail::type_error &err) {
                    user["night_counts"]=1;
                }
                try {
                    user["night_sum"]=user["night_sum"].get<long long>()+t.tm_hour*3600+t.tm_min*60+t.tm_sec;
                } catch (nlohmann::detail::type_error &err) {
                    user["night_sum"]=t.tm_hour*3600+t.tm_min*60+t.tm_sec;
                }

                data[to_string(event.user_id)]=user;
                os << data.dump(4) << endl;
                os.close();

                logging::info("晚安",to_string(event.user_id)+"成功晚安");

                Message msg=reply[0];
                
                int day_lstyear,day_lstmonth,day_lstday;
                time_t day_lastt;
                get_data(1,day_lstyear,day_lstmonth,day_lstday,day_lastt,event);
                double difsecs=difftime(nowt,day_lastt);
                if(difsecs>86400) {
                    msg=config["night_without_morning"].as<string>();
                } else {
                    int hours=(int)difsecs/3600;
                    int minutes=((int)difsecs-hours*3600)/60;
                    int seconds=(int)difsecs%60;
                    msg=replace_all_distinct(msg,"${time_day}",to_string(hours)+"时"+to_string(minutes)+"分"+to_string(seconds)+"秒");
                }
                
                if(msg=="null")return true;

                msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
                msg=replace_all_distinct(msg,"${ranking}",to_string(night_ranking));
                GroupMember gm=get_group_member_info(event.group_id,event.user_id);
                string call;
                if(gm.sex==cq::Sex::FEMALE)call=config["call"]["females"].as<string>();
                else if(gm.sex==cq::Sex::MALE)call=config["call"]["males"].as<string>();
                else call=config["call"]["default"].as<string>();
                msg=replace_all_distinct(msg,"${call}",call);

                send_group_message(event.group_id,msg);
            }
        } catch (ApiError &err) {} //忽略错误
    } else if(eventtype==1) { //早安
        get_data(1,lastyear,lastmonth,lastday,day_lastt,event);

        int accept_start_hour=config["time"]["morning"]["accept_start_hour"].as<int>(),accept_end_hour=config["time"]["morning"]["accept_end_hour"].as<int>();

        if((accept_start_hour<accept_end_hour&&(t.tm_hour<accept_start_hour||t.tm_hour>=accept_end_hour)) || (accept_start_hour>=accept_end_hour&&(t.tm_hour<accept_start_hour&&t.tm_hour>=accept_end_hour))) { //不在早安区间
            Message msg=config["out_of_time_period"]["morning"].as<string>();
            if(msg=="null")return true;
            msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
            msg=replace_all_distinct(msg,"${start_time}",to_string(config["time"]["morning"]["accept_start_hour"].as<int>()));
            msg=replace_all_distinct(msg,"${start_time}",to_string(config["time"]["morning"]["accept_end_hour"].as<int>()));
            send_group_message(event.group_id,msg);
            return true;
        }

        try {
            if(lastyear==nowyear && lastmonth==nowmonth && lastday==nowday) { //今天已经早安过了
                Message msg=config["multi"]["morning"].as<string>();
                logging::info("早安",to_string(event.user_id)+"重复早安");
                if(msg=="null")return true;
                msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
                send_group_message(event.group_id,msg);
            } else {
                increase_ranking(data,1,event);

                ofstream os(ansi(dir::app()+"groups\\"+to_string(event.group_id)+".json"));
                json user=data[to_string(event.user_id)];
                user["day_lasttime"]=time(NULL);
                try {
                    user["day_counts"]=user["day_counts"].get<int>()+1;
                } catch (nlohmann::detail::type_error &err) {
                    user["day_counts"]=1;
                }
                try {
                    user["day_sum"]=user["day_sum"].get<long long>()+t.tm_hour*3600+t.tm_min*60+t.tm_sec;
                } catch (nlohmann::detail::type_error &err) {
                    user["day_sum"]=t.tm_hour*3600+t.tm_min*60+t.tm_sec;
                }

                data[to_string(event.user_id)]=user;
                os << data.dump(4) << endl;
                os.close();

                logging::info("早安",to_string(event.user_id)+"成功早安");

                Message msg=reply[1];
                
                int day_lstyear,day_lstmonth,day_lstday;
                time_t day_lastt;
                get_data(0,day_lstyear,day_lstmonth,day_lstday,day_lastt,event);
                double difsecs=difftime(nowt,day_lastt);
                if(difsecs>86400) {
                    msg=config["morning_without_night"].as<string>();
                } else {
                    int hours=(int)difsecs/3600;
                    int minutes=((int)difsecs-hours*3600)/60;
                    int seconds=(int)difsecs%60;
                    msg=replace_all_distinct(msg,"${time_day}",to_string(hours)+"时"+to_string(minutes)+"分"+to_string(seconds)+"秒");
                }
                
                if(msg=="null")return true;

                msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
                msg=replace_all_distinct(msg,"${ranking}",to_string(day_ranking));
                GroupMember gm=get_group_member_info(event.group_id,event.user_id);
                string call;
                if(gm.sex==cq::Sex::FEMALE)call=config["call"]["females"].as<string>();
                else if(gm.sex==cq::Sex::MALE)call=config["call"]["males"].as<string>();
                else call=config["call"]["default"].as<string>();
                msg=replace_all_distinct(msg,"${call}",call);

                send_group_message(event.group_id,msg);
            }
        } catch (ApiError &err) {} //忽略错误
    } else if(eventtype==2) { //查询作息数据
        Message msg=reply[2];
        if(msg=="null")return true;
        
        msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
        msg=replace_all_distinct(msg,"${number_asleep}",to_string(night_ranking));
        msg=replace_all_distinct(msg,"${number_awake}",to_string(day_ranking));

        send_group_message(event.group_id,msg);
    } else if(eventtype==3) { //查询我的数据
        Message msg=reply[3];
        if(msg=="null")return true;
        
        try {
            msg=replace_all_distinct(msg,"${at}",MessageSegment::at(event.user_id));
            int night_counts=data[to_string(event.user_id)]["night_counts"].get<int>(),day_counts=data[to_string(event.user_id)]["day_counts"].get<int>();
            msg=replace_all_distinct(msg,"${number_asleep}",to_string(night_counts));
            msg=replace_all_distinct(msg,"${number_awake}",to_string(day_counts));
            long long night_sum=data[to_string(event.user_id)]["night_sum"].get<long long>(),day_sum=data[to_string(event.user_id)]["day_sum"].get<long long>();
            double night_secs=(double)night_sum/night_counts,day_secs=(double)day_sum/day_counts;
            int night_hours=(int)night_secs/3600;
            int night_minutes=((int)night_secs-night_hours*3600)/60;
            int night_seconds=(int)night_secs%60;
            int day_hours=(int)day_secs/3600;
            int day_minutes=((int)day_secs-day_hours*3600)/60;
            int day_seconds=(int)day_secs%60;
            msg=replace_all_distinct(msg,"${avg_sleep}",to_string(night_hours)+"时"+to_string(night_minutes)+"分"+to_string(night_seconds)+"秒");
            msg=replace_all_distinct(msg,"${avg_wake}",to_string(day_hours)+"时"+to_string(day_minutes)+"分"+to_string(day_seconds)+"秒");
            GroupMember gm=get_group_member_info(event.group_id,event.user_id);
            string call;
            if(gm.sex==cq::Sex::FEMALE)call=config["call"]["females"].as<string>();
            else if(gm.sex==cq::Sex::MALE)call=config["call"]["males"].as<string>();
            else call=config["call"]["default"].as<string>();
            msg=replace_all_distinct(msg,"${call}",call);

            send_group_message(event.group_id,msg);
        } catch (nlohmann::detail::type_error &err) {
            send_group_message(event.group_id,MessageSegment::at(event.user_id)+"您还没有早安或没有晚安过哦~");
        }
    }

    return false;
}