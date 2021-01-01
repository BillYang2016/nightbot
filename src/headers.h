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

#define COMMAND_AMOUNT 4

#define HEADERS

#else

#define HEADERS extern

#endif