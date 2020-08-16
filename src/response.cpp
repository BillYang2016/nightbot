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

bool Response(const int &id,const GroupMessageEvent &event) {
	return false;
}