#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

source $SCRIPT_DIR/../common/log.sh
    
# 检测tailscale是否安装 
if ! command -v tailscale >/dev/null; then
    log_warning "Tailscale工具安装中"
    curl -fsSL https://tailscale.com/install.sh | sh

    if [ $? -eq 0 ]; then
        log_info "Tailscale 安装成功"
    else
        log_error "Tailscale 安装失败"
        exit 1
    fi
fi


# 运行tailscale 应用
token=$1
if [[ -z $token ]]; then
    log_error "请输出: ./tailscale.sh tskey-auth-xxxxx"
    exit 1
fi

sudo tailscale up --authkey $token
if [ $? -eq 0 ]; then
    log_info "Tailscale 运行成功"
else
    log_error "Tailscale 执行失败"
fi


