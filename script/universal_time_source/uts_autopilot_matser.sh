#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
function log_info() {
  echo -e "\033[32m[INFO] $*\033[0m[\033[32m\u2714\033[0m]"
}

function log_warning() {
  echo -e "\033[33m[WARNING] $*\033[0m[\033[33m⚠️\033[0m]"
}

function log_error() {
  echo -e "\033[31m[ERROR] $*\033[0m[\033[31m\xE2\x9C\x97\033[0m]"
}

log_info "linuxptp开始安装中..."
sudo apt install linuxptp -y
if [ $? -eq 0 ]; then
    log_info "linuxptp安装成功"
else
    log_error "linuxptp安装失败"
    exit 1
fi
sudo systemctl daemon-reload

log_info "ptp4l@.service"
cat << EOF > ptp4l@.service
[Unit]
Description=Precision Time Protocol (PTP) service
After=network-online.target

[Service]
Type=simple
ExecStart=/usr/sbin/ptp4l -i %i -m -S -A -s
ExecStop=/usr/bin/pkill -f ptp4l
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target network-online.target
EOF

sudo mv ptp4l@.service /etc/systemd/system/ptp4l@.service


# sudo systemctl start ptp4l@eno1.service
# sudo systemctl start ptp4l@enp3s0.service
# sudo systemctl enable ptp4l@enp3s0.service