# livox 激光雷达ip修改程序

## 概要
用于方便快捷的修改Livox激光雷达的IP

## 使用步骤
### step-1: 确定Lidar联通工控机
Livox激光雷出厂IP`192.168.1.1xx`(xx为LivoxSN码最后两位)，比如47MDL9J0020005 -> 192.168.1.105
```shell
ping 192.168.1.1xx
```
### step-2: 开始修改
./update_ipv4 [config file] [SN] [LidarUpdataIp]
- config file    Lidar的配置文件
- SN:            Lidar的SN
- LidarUpdataIp: 想要修改的目标IP

例如`./update_ipv4.run mid360_config.json 47MDL9J0020005 192.168.1.105`
- mid360_config.json: mid360型号Lovix Lidar
- 47MDL9J0020005:     mid360型号Lovix Lidar的SN
- 192.168.1.105:      想要修改的目标IP

### step-2: 成功标志
使用ping指令，是否联通
```shell
ping [LidarUpdataIp]
```

## 引用
[修改使用的API](https://github.com/Livox-SDK/Livox-SDK2/blob/f1445aaeb4632195f97ad6cb4cbe9efda96f0fb2/samples/livox_lidar_quick_start/main.cpp#L205C16-L205C16)