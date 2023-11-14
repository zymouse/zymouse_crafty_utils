#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
source $SCRIPT_DIR/log.sh

function create_rules(){
    yq  '.[] | [.dev_path, .ln_name]' "$SCRIPT_DIR/create_rule_files.yaml" | while read -r dev_path;read -r ln_name
    do
        dev_path="${dev_path//- /}"
        SYMLINK="${ln_name//- /}"
        
        if [ -z "$SYMLINK" ];then
            log_error "请输入[./create_rule_files.sh /dev/video0 camera/front_6mm]"
            exit 1
        fi
        if [ -z "$dev_path" ];then
            log_error "请输入[./create_rule_files.sh /dev/video0 camera/front_6mm]"
            exit 1
        fi

        SUBSYSTEM=$(udevadm info --attribute-walk --name=$dev_path  |grep 'SUBSYSTEM==' -m 1 | xargs)
        SUBSYSTEMS=$(udevadm info --attribute-walk --name=$dev_path  |grep 'SUBSYSTEMS=='  -m 1 | xargs)
        KERNELS=$(udevadm info --attribute-walk --name=$dev_path  |grep 'KERNELS==' -m 1 | xargs)
        ATTRS_idVendor=$(udevadm info --attribute-walk --name=$dev_path  |grep 'ATTRS{idVendor}=='  -m 1 | xargs)
        ATTRS_idProduct=$(udevadm info --attribute-walk --name=$dev_path  |grep 'ATTRS{idProduct}=='  -m 1 | xargs)


        echo "$SUBSYSTEM, $SUBSYSTEMS, $KERNELS, $ATTRS_idVendor, $ATTRS_idProduct, SYMLINK+=\"$SYMLINK\""
        
        if [ -z "$SUBSYSTEM" ] && [ -z "$SUBSYSTEMS" ]; then
            return 1
        fi
    done
}

echo -----------------------------------------------------------------------------------------------------------------------
create_rules
if [ $? -eq 0 ]; then
    log_info "规则文件生成成功"
else
    log_error "规则文件生成失败"
fi

echo -----------------------------------------------------------------------------------------------------------------------
log_info "cmd > \nsudo vim /etc/udev/rules.d/99-webcam.rules \nsudo udevadm control --reload-rules \nsudo udevadm trigger\n"