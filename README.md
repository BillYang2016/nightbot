# Night bot

当前版本号：1.0.0

## 联系方式
[QQ群 735209171](https://jq.qq.com/?_wv=1027&k=5cV7uEJ)  

## 插件依赖
本插件依赖cqcppsdk进行构建，如果在Mirai上运行需要使用Mirai-Native

## 插件介绍
这是一款让你在群聊里晚安早安并获得反馈的插件，让你的群活跃起来哦~
目前支持功能如下：
1. 晚安反馈
2. 早安反馈
3. 统计今日数据
4. 统计平均数据

## 插件使用
~~由于本插件尚未加入GUI，所以~~请在应用目录（CoolQ/data/app/com.billyang.nightbot/）内进行应用配置：  
1. 将app放入CoolQ/app/文件夹（使用Mirai请参考Mirai-Native配置方法）
2. 重新载入应用
3. 启用本应用（打开`config.yml`并将`enable`项改为`true`）
4. 打开`config.yml`并在`groups`项输入启用群号，使用','进行分割
5. 恭喜，插件可以正常运行啦！

## 默认指令
1. night: `晚安`
2. moring: `早安`、`早上好`
3. todaydata: `作息数据`
4. mydata: `我的数据`

## 扩展设置
打开`config.yml`，下面依次介绍：
```yml
enable: false # 改为true后 启用插件
groups: 0 # 输入群号，使用','分割
admin: -1 # 所有者账号，可以填自己大号
command:
  night: # 晚安的指令
    keyword: # 默认指令为“晚安”，添加以“- ”开头的新行加入新的同功能指令
      - 晚安
    match: 0 # 匹配模式：0为精确匹配（必须和keyword完全相同） 1为模糊匹配（输入包含keyword触发）
    priority: 0 # 权限需求：0为任何人均可使用，1为仅管理员可使用
    reply: ${at}晚安哦，你今天的清醒时间为${time_day}，是第${ranking}个睡觉的${call}~ # 回复方式：${at}为at群友，${time_day}为从上次晚安到本次早安所经过时间（24小时内），${ranking}为已经晚安过的人数，${call}为性别化称呼，在后面编辑
  morning:
    keyword:
      - 早安
      - 早上好
    match: 0
    priority: 0
    reply: ${at}早上好，你昨天的睡眠时间为${time_day}，是第${ranking}个起床的${call}~ # 回复方式：与晚安指令对称
  todaydata:
    keyword:
      - 作息数据
    match: 0
    priority: 0
    reply: ${at}今天有${number_asleep}个群友睡觉，${number_awake}个群友起床。 # 回复方式：${number_asleep}为已经晚安过的人数，${number_awake}为已经早安过的人数
  mydata:
    keyword:
      - 我的数据
    match: 0
    priority: 0
    reply: ${at}你睡觉了${number_asleep}次，起床了${number_awake}次，平均在${avg_sleep}睡觉，平均在${avg_wake}起床。 # 回复方式：${number_asleep}为已经晚安过的人数，${number_awake}为已经早安过的人数，${avg_sleep}为个人平均睡觉时间，${avg_wake}为个人平均起床时间
night_without_morning: ${at}晚安哦，您是今天第${ranking}个睡觉的群友~ # 在没有早安数据时晚安的回复，此处也可使用${call}
morning_without_night: ${at}早上好，您是今天第${ranking}个起床的群友~ # 在没有晚安数据时早安的回复，此处也可使用${call}
call: # 性别化称呼
  males: 少年
  females: 少女
  default: 少女
```

## 插件构建
本插件依赖于[cqcppsdk](https://cqcppsdk.cqp.moe/)，请先了解SDK的相关知识  

1. 安装cqcppsdk依赖
2. 安装yaml-cpp依赖
3. 使用cmake工具进行构建
4. 使用vs进行release的发布，获得dll后载入酷Q

## 问题反馈
如果在使用中遇到问题、bug，或者有其他建议，欢迎通过[issue](https://github.com/BillYang2016/nightbot/issues)反馈，或者进入交流群进行交流，非常感谢  

## PR审核
请注意以下事项，否则我们可能会直接关闭你的PR：  

1. 请保证代码风格的一致
2. 请注意异常处理
3. 请注意添加注释
4. 请言简意赅地说明PR修改