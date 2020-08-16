#pragma once

#include "headers.h"

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

string replace_all(string str,const string& old_value,const string& new_value) {
    while(true) {
        string::size_type pos(0);
        if((pos=str.find(old_value))!=string::npos)str.replace(pos,old_value.length(),new_value);
        else break;
    }
    return str;
}

string replace_all_distinct(string str,const string& old_value,const string& new_value) {
    for(string::size_type pos(0); pos!=string::npos; pos+=new_value.length()) {
        if((pos=str.find(old_value,pos))!=string::npos)str.replace(pos,old_value.length(),new_value);
        else break;
    }
    return str;
}