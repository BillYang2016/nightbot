# Night bot

当前版本号：1.0.6

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
6. 如果控制台显示插件返回126报错，那么你可能需要安装VC运行库（vc_redist.x86）

## 更新须知
> 目前的更新方案仅暂时使用，后续会采用更方便的更新适配方案。

请在更新插件版本后，备份并删除`config.yml`，然后重启CoolQ/Mirai，插件会重新生成`config.yml`，请按照新的配置文件重新配置。
重新配置完毕后保存`config.yml`，并重启CoolQ/Mirai，完成对插件的更新。

## 默认指令
1. night: `晚安`
2. moring: `早安`、`早上好`
3. todaydata: `作息数据`
4. mydata: `我的数据`

## 扩展设置
打开`config.yml`，下面依次介绍：
```yml
enable: false # 改为true后 启用插件
groups: 0 # 输入群号，使用','分割（填写'1'表示全体启用）
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
multi: # 重复早晚安回复
  morning: ${at}您今天已经早安过了哦~
  night: ${at}您今天已经晚安过了哦~
out_of_time_period: # 不在早晚安时间区间回复
  morning: ${at}您不在早安时间区间内哦，允许早安时间：${start_time}时~${end_time}时。
  night: ${at}您不在晚安时间区间内哦，允许晚安时间：${start_time}时~${end_time}时。
short_inteval: # 过短的睡眠/清醒时间回复
  morning: ${at}只睡这么会儿你不怕猝死？
  night: ${at}睡你麻痹起来嗨！
call: # 性别化称呼（目前处于失效状态）
  males: 少年
  females: 少女
  default: 少女
time:
  start_hour: 6 # 跨日时间（默认早上6点）
  morning: # 允许早安区间（0~24时）
    accept_start_hour: 0
    accept_end_hour: 24
  night: # 允许晚安区间（0~24时）
    accept_start_hour: 0
    accept_end_hour: 24
  least_asleep: 6 # 最少睡眠时间（时）
  least_awake: 6 # 最少清醒时间（时）
  cd: # 两次指令CD（时）
    morning: 12
    night: 12
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