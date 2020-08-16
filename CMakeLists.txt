cmake_minimum_required(VERSION 3.9)
project(wikibot) # 填写项目名称

set(CMAKE_CXX_STANDARD 17) # 设置 C++ 标准版本为 C++17, 必须

include(extern/cqcppsdk/cqcppsdk.cmake) # 包含 SDK 的 CMake 脚本, 必须

option(BUILD_SHARED_LIBS "" OFF) # 设置编译静态库而不是默认的动态库
add_subdirectory(extern/yaml) # 添加 CMake 子目录

include_directories(extern/yaml/include) # 添加包含目录

add_definitions(-DCURL_STATICLIB) # 设置使用静态库

include_directories(src) # 设置 src 目录为包含目录, 可根据实际情况修改

link_directories(lib) # Mirai-Native-API

# 从 app_id.txt 文件读取 app id, 也可以直接设置
file(READ "app_id.txt" APP_ID)
string(STRIP "${APP_ID}" APP_ID)
cq_set_app_id(${APP_ID}) # 设置 app id, 必须

file(GLOB_RECURSE SOURCE_FILES src/*.cpp) # 递归搜索 src 目录中的源文件, 可根据实际情况修改

if (CQ_CAN_BUILD_STD_MODE)
    cq_add_std_app(${SOURCE_FILES}) # 添加 std 模式的动态链接库构建目标
    target_link_libraries(app yaml-cpp)
    cq_install_std_app()
endif ()

# 添加 dev 模式的可执行文件构建目标
cq_add_dev_app(${SOURCE_FILES})
target_link_libraries(app_dev yaml-cpp)
