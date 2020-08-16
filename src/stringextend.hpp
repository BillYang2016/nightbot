#pragma once

#include <sstream>
#include <vector>

vector<string> stringSplit(const string &str,const string &pattern);

string replace_all(string str,const string& old_value,const string& new_value);

string replace_all_distinct(string str,const string& old_value,const string& new_value);