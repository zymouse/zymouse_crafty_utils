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

exit_node_ip=$1
if [[ -z $exit_node_ip ]]; then
    log_warning "代理已取消, 现在无法ping通[ ping www.google.com ]"
else
    log_info "代理已设置, 现在可以ping通[ ping www.google.com ]"
fi
sudo tailscale set --exit-node=$exit_node_ip --exit-node-allow-lan-access=true

# 设置代理: ./tailscale_exit_node.sh <exit-node-ip>
# 取消代理: ./tailscale_exit_node.sh