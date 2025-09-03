#!/bin/bash
set -e

if [ "$EUID" -ne 0 ]; then
  echo "[ERROR] 请用 sudo 运行此脚本！"
  exit 1
fi

echo "[INFO] 正在以 root 权限运行。"

SCRIPT_PATH="/opt/time_jump.py"
SERVICE_PATH="/etc/systemd/system/time_jump.service"

read -p "安装还是卸载(Y/N): " answer
answer=$(echo "$answer" | tr 'a-z' 'A-Z')  # 转换为大写（防止小写 y）
if [ "$answer" == "Y" ]; then
  echo "开始安装"
elif [ "$answer" == "N" ]; then
  echo "开始卸载"
  systemctl disable time_jump.service
  systemctl stop time_jump.service
  rm $SERVICE_PATH
  echo "卸载完成: sudo systemctl status time_jump.service "
  exit 0
else
  echo "输入无效，请输入 Y 或 N。"
  exit 1
fi



# 拷贝监控脚本
cp ./time_jump.py $SCRIPT_PATH
chmod +x $SCRIPT_PATH

# 写 service 文件
cat > $SERVICE_PATH <<EOF
[Unit]
Description=System Time Jump Monitor
After=network.target

[Service]
ExecStart=/usr/bin/python3 $SCRIPT_PATH
Restart=on-failure
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

# 启动服务
systemctl daemon-reload
systemctl enable time_jump.service
systemctl restart time_jump.service

echo "[OK] time_jump 已部署并开机自启"
echo " sudo systemctl status time_jump.service "