#pragma once

#include "headers.h"

string names[COMMAND_AMOUNT+1];
vector<string> commands[COMMAND_AMOUNT+1];
string reply[COMMAND_AMOUNT+1];
int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员