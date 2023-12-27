#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
source $SCRIPT_DIR/script/create_rule_files/log.sh


function install_yq() {
    version_info=$(yq --version)
    if [ $? -eq 0 ]; then
        extracted_version=$(echo $version_info | grep -oP 'version \Kv[0-9.]+')
    fi
    extracted_version=""

    set -e #如果任何语句的执行结果不是真（即返回非零退出状态）
    arch=$(uname -m)

    case "$arch" in
        x86_64)
            arch=amd64
            ;;
        aarch64)
            arch=arm64
            ;;
        *)
            echo "This system has a different architecture: $arch"
            exit 0
            ;;
    esac

    # 判断版本号是否为 v4.35.2
    if [ "$extracted_version" != "$VERSION" ]; then
        return 0
    fi

    VERSION=v4.35.2 
    BINARY=yq_linux_$arch

    wget https://github.com/mikefarah/yq/releases/download/${VERSION}/${BINARY} &&\
    sudo mv ${BINARY} /usr/bin/yq &&\
    sudo chmod +x /usr/bin/yq
    return 0
}


install_yq
if [ $? -eq 0 ]; then
    log_info "step-1 : yq部署成功"
else
    log_error "step-1 : yq部署失败"
fi

python3 -m pip install evdev
if [ $? -eq 0 ]; then
    log_info "step-2 : evdev部署成功"
else
    log_error "step-2 : evdev部署失败"
fi

python3 -m pip install pyproj
if [ $? -eq 0 ]; then
    log_info "step-3 : pyproj部署成功"
else
    log_error "step-3 : pyproj部署失败"
fi
