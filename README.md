# 公主连结黄历

当前版本号：pcrcalendar 1.0.2

## 联系方式
[QQ群 735209171](https://jq.qq.com/?_wv=1027&k=5cV7uEJ)  
~~[酷Q帖子](https://cqp.cc/t/51730)~~  

## 插件介绍
很简单的插件，每天签到获取~~阴间~~运势：  
![阴间运势](/docs/demo.png)  

## 插件使用
~~由于本插件尚未加入GUI，所以~~请在应用目录（CoolQ/data/app/com.billyang.calendar/）内进行应用配置：  
1. 将app放入CoolQ/app/文件夹（使用Mirai请参考Mirai-Native配置方法）
2. 重新载入应用
3. 启用本应用（打开`config.yml`并将`enable`项改为`true`）
4. 打开`config.yml`并在`groups`项输入启用群号，使用','进行分割
5. 恭喜，插件可以正常运行啦！

```diff
-enable: false
+enable: true
-groups: 0
+groups: groupnumber1,groupnumber2,...
```

## 扩展设置
本插件的数据均均匀随机生成自应用目录中的`data.json`（大凶和大吉需要进行二次鉴定，并有独特的宜忌事件说明）  
插件在第一次启用时会自动生成默认`data.json`  

该json的架构大致如下：
- `"fortune": ["大吉",...]  //用于生成运势`
- `"characters": ["望",...]  //用于生成默认角色`
- `"suitable": {"key": "value",...}  //键值对用于生成"宜"事件及说明`
- `"unsuitable": {"key": "value",...}  //键值对用于生成"忌"事件及说明`
- `"position": ["面向东方",...]  //用于生成加成方位`
- `"actions": ["躺在地上",...]  //用于生成加成动作`

可以任意按照json格式添加数据来增加对应事件  

在`config.yml`中可以通过修改`command`项从而编辑命令
```yml
command:
  register: 签到
  query: 运势
```

**注意：**
1. 请尽量保持本插件运行环境内所有文件为UTF-8编码
2. 请保证键值对`suitable`与`unsuitable`的键顺序相同

## 插件构建
本插件依赖于[cqcppsdk](https://cqcppsdk.cqp.moe/)，请先了解SDK的相关知识  

1. 安装cqcppsdk依赖
2. 安装yaml-cpp依赖
3. 使用cmake工具进行构建
4. 使用vs进行release的发布，获得dll后载入酷Q

## 问题反馈
如果在使用中遇到问题、bug，或者有其他建议，欢迎通过[issue](https://github.com/BillYang2016/pcrcalendar-qqbot/issues)反馈，或者进入交流群进行交流，非常感谢  

## PR审核
请注意以下事项，否则我们可能会直接关闭你的PR：  

1. 请保证代码风格的一致
2. 请注意异常处理
3. 请注意添加注释
4. 请言简意赅地说明PR修改