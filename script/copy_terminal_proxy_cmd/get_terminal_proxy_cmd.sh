#!/bin/bash

# 获取特定网络接口的 IPv4 地址
# 例如，假设你的网络接口是 eth0，你可以根据实际情况修改它
INTERFACE="eth0"
IP=$(ip addr show $INTERFACE | grep 'inet ' | awk '{print $2}' | cut -d/ -f1)

# 检查是否成功获取到 IP 地址
if [ -z "$IP" ]; then
    echo "无法获取 IP 地址"
    exit 1
fi

# 构造 export 命令
EXPORT_CMD="export https_proxy=http://$IP:7890 http_proxy=http://$IP:7890 all_proxy=socks5://$IP:7890"

# 打印出执行的命令，以便确认
echo $EXPORT_CMD
