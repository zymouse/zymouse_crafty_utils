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
    
# 检测tailscale是否安装 
log_info "1.0 检测tailscale是否安装"
if ! command -v tailscale >/dev/null; then
    log_warning "Tailscale工具安装中"
    curl -fsSL https://tailscale.com/install.sh | sh

    if [ $? -ne 0 ]; then
        log_error "Tailscale 安装失败:[建议使用clash局域网代理执行]"
        exit 1
    fi
fi
log_info "2.0 检测tailscale已安装"

# 运行tailscale 应用
log_info "3.0 tailscale应用 开始运行"
token=$1
if [[ -z $token ]]; then
    log_error "请输出: ./tailscale.sh tskey-auth-xxxxx"
    exit 1
fi

if [[ "$token" != tskey-auth-* ]]; then
    log_error "$token 不是'tskey-auth-....'开头的正确密钥 "
    exit 1
fi


sudo tailscale up --authkey $token
if [ $? -eq 0 ]; then
    log_info "4.0 Tailscale应用 运行成功"
else
    log_error "Tailscale应用 运行失败"
fi


# 如何使用
# ./tailscale.sh <token-key>