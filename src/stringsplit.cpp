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

#ifndef HEADERS

using json = nlohmann::json;
using node = YAML::Node;

using cq::utils::ansi;

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

#define HEADERS

#endif

vector<string> stringSplit(const string &str,const string &pattern) {
    vector<std::string> resVec;
    if ("" == str)return resVec;
    string strs = str + pattern;
    size_t pos = strs.find(pattern);
    size_t size = strs.size();
    while (pos != string::npos){
        string x = strs.substr(0,pos);
        resVec.push_back(x);
        strs = strs.substr(pos+1,size);
        pos = strs.find(pattern);
    }
    return resVec;
}