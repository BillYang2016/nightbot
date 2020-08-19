# Issue bot

当前版本号：1.0.1

## 联系方式
[QQ群 735209171](https://jq.qq.com/?_wv=1027&k=5cV7uEJ)  

## 插件依赖
本插件依赖cqcppsdk进行构建，如果在Mirai上运行需要使用Mirai-Native

## 插件介绍
这是一款让你在群聊里管理issue的插件，可以让你群无法上github的群友快速在群反馈bug/提出需求/提问  
目前支持功能如下：
1. 新增issue
2. 开启/关闭 issue
3. 回复issue
4. 搜索/全部 issue
5. 添加/删除 tag
6. 指派/取消指派
7. 查看issue

附加功能：
1. 自动同意好友请求
2. 将issue的实时进展私聊推送给所有参与者
3. 实时监测群聊消息，识别提问自动反馈匹配程度最高issue（测试中，现方案为lcs，后续进行优化）

## 插件使用
~~由于本插件尚未加入GUI，所以~~请在应用目录（CoolQ/data/app/com.billyang.issuebot/）内进行应用配置：  
1. 将app放入CoolQ/app/文件夹（使用Mirai请参考Mirai-Native配置方法）
2. 重新载入应用
3. 启用本应用（打开`config.yml`并将`enable`项改为`true`）
4. 打开`config.yml`并在`groups`项输入启用群号，使用','进行分割
5. 恭喜，插件可以正常运行啦！

## 默认指令
1. add：`添加issue#${issue标题}#${详细内容}` 添加一个新的issue
2. close：`关闭issue#${issue标题或id}` 将一个issue的状态设置为`Closed`
3. reply：`回复issue#${issue标题或id}#${详细内容}` 回复一个issue
4. search：`搜索issue#${关键词、状态或tag}#${页码，选填}` 搜索所有包含关键词的或状态相同的或包含指定tag的issue，页码默认为1
5. reopen：`重开issue#${issue标题或id}` 将一个issue的状态设置为`Open`
6. all：`全部issue#${页码，选填}` 查询所有issue，页码默认为1
7. newtag：`添加tag#${issue标题或id}#${tag内容}` 为issue添加一个tag
8. assign：`指派#${issue标题或id}#${@群员或输入qq号}` 为issue指派群员
9. view：`查看issue#${issue标题或id}` 查看一个issue的详细信息
10. deltag：`删除tag#${issue标题或id}#${tag内容}` 为issue删除一个tag
11. unassign：`取消指派#${issue标题或id}#${@群员或输入qq号}` 为issue取消指派群员

## 扩展设置
打开`config.yml`，下面依次介绍：
```yml
enable: false # 改为true后 启用插件
groups: 0 # 输入群号，使用','分割
admin: -1 # 所有者账号，可以填自己大号（接受推送）
friendrequest: 0 # 改为1后 自动通过所有好友申请
command: # 命令列表
  add:
    keyword: 添加issue # 修改此项可修改插件触发关键词
    match: 1 # 匹配模式：0为精确匹配（必须和keyword完全相同） 1为模糊匹配（输入包含keyword触发）
    priority: 0 # 权限需求：0为任何人均可使用，1为仅管理员与issue提出者可使用，2为仅管理员可使用
  close:
    keyword: 关闭issue
    match: 1
    priority: 1
  reply:
    keyword: 回复issue
    match: 1
    priority: 0
  search:
    keyword: 搜索issue
    match: 1
    priority: 0
  reopen:
    keyword: 重开issue
    match: 1
    priority: 1
  all:
    keyword: 全部issue
    match: 0
    priority: 0
  newtag:
    keyword: 添加tag
    match: 1
    priority: 1
  assign:
    keyword: 指派
    match: 1
    priority: 2
  view:
    keyword: 查看issue
    match: 1
    priority: 0
  deltag:
    keyword: 删除tag
    match: 1
    priority: 1
  unassign:
    keyword: 取消指派
    match: 1
    priority: 2
notification: # 修改此项可修改插件的私聊推送内容
  newissue: "您在${group}群有新的issue啦！\n${title} (#${id})\n${content}\n时间：${time}\n发起者：${author}" # ${group}：群号，${title}：issue标题，${id}：issue的id，${content}：第一层内容
  closed: "${group}群的issue被${author}关闭啦！\n${title} (#${id})\n时间：${time}"
  reply: "${group}群的issue被${author}回复啦！\n${title} (#${id})\n${content}\n时间：${time}" # ${content}：新的一层的内容
  reopened: "${group}群的issue被${author}重新开启啦！\n${title} (#${id})\n时间：${time}"
  newtag: "${author}为${group}群的issue添加了标签！\n${title} (#${id})\n${content}\n时间：${time}" # ${content}：一般格式为 *Added tag "tag内容"，后类似
  deltag: "${author}为${group}群的issue删除了标签！\n${title} (#${id})\n${content}\n时间：${time}"
  assign: "${author}为${group}群的issue添加了指派！\n${title} (#${id})\n${content}\n时间：${time}"
  unassign: "${author}为${group}群的issue取消了指派！\n${title} (#${id})\n${content}\n时间：${time}"
reply: # 命令回复
  view: "${at}\n${title} (#${id}) [${status}] (${pagenum}页)\nTags: ${tags}\nAssignees: ${assignees}\n----------\n${vieweach}" # ${at}：@发送指令的人，${status}：Open还是Closed，${pagenum}：当前页码/页数（1/3），${tags}：所有tag，${assignees}：所有assignee（显示昵称和qq号），${vieweach}：所有issue内容，每一条的格式见下面的vieweach
  search: "${at}\n找到以下issue：(${pagenum}页)\n${searcheach}" # ${searcheach}：所有search结果，每一条的格式见下面的searcheach
  vieweach: "${content}\n作者：${author}\n时间：${time}\n----------"
  searcheach: "- ${title} (#${id}) [${status}] ${tags}"
  view_perpage: 3 # 每一页显示多少层（多了容易被吞）
  search_perpage: 5 # 每一页显示多少个结果（多了容易被吞）
detect:
  enable: false # 改成true启用自动监测反馈相似issue
  limit: 60 # 相似度>=limit%将反馈相似issue，另外如果消息中出现了keywords.json中的词，无论相似度如何都会进行反馈
  reply: "${at}为您找到最为匹配的issue，匹配度${score}：\n${title} (#${id}) [${status}]\n${content}\n请输入\"查看issue#${id}\"以获取详细信息！" # 支持${at}、${title}、${id}、${status}、${content}、${author}、${time}、${score}、${tags}，其中${content}、${author}、${time}均为第一层
```

## 插件构建
本插件依赖于[cqcppsdk](https://cqcppsdk.cqp.moe/)，请先了解SDK的相关知识  

1. 安装cqcppsdk依赖
2. 安装yaml-cpp依赖
3. 使用cmake工具进行构建
4. 使用vs进行release的发布，获得dll后载入酷Q

## 问题反馈
如果在使用中遇到问题、bug，或者有其他建议，欢迎通过[issue](https://github.com/BillYang2016/issuebot/issues)反馈，或者进入交流群进行交流，非常感谢  

## PR审核
请注意以下事项，否则我们可能会直接关闭你的PR：  

1. 请保证代码风格的一致
2. 请注意异常处理
3. 请注意添加注释
4. 请言简意赅地说明PR修改