#pragma once

#include <iostream>
#include <cstring>

using namespace std;

#define COMMAND_AMOUNT 8

static string names[COMMAND_AMOUNT+1];
static string commands[COMMAND_AMOUNT+1];
static int match_method[COMMAND_AMOUNT+1]; //1模糊 0精确
static int priority_requied[COMMAND_AMOUNT+1]; //0全体成员 1管理员或发起者 2仅管理员

static void commands_init() {
	names[0]="add",commands[0]="添加issue",match_method[0]=1,priority_requied[0]=0;
	names[1]="close",commands[1]="关闭issue",match_method[1]=1,priority_requied[1]=1;
	names[2]="reply",commands[2]="回复issue",match_method[2]=1,priority_requied[2]=0;
	names[3]="search",commands[3]="搜索issue",match_method[3]=1,priority_requied[3]=0;
	names[4]="reopen",commands[4]="重开issue",match_method[4]=1,priority_requied[4]=1;
	names[5]="all",commands[5]="全部issue",match_method[5]=0,priority_requied[5]=0;
	names[6]="newtag",commands[6]="添加tag",match_method[6]=1,priority_requied[6]=1;
	names[7]="assign",commands[7]="指派issue",match_method[7]=1,priority_requied[7]=2;
}

static int get_id_by_name(string name) {
	for(int i=0; i<COMMAND_AMOUNT; i++)if(name==names[i])return i;
	return -1;
}