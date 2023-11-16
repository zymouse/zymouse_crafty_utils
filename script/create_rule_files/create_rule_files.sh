#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
source $SCRIPT_DIR/log.sh

function fix_subsystem_string() {
  # 输入参数为原始字符串
  original_string="$1"

  # 使用sed替换操作将==替换为==
  fixed_string=$(echo "$original_string" | sed 's/==/=="/')
  fixed_string=$fixed_string'"'

  # 返回修正后的字符串
  echo "$fixed_string"
}
function fix_space_string() {
  # 输入参数为原始字符串
  original_string="$1"

  # 使用sed去除开头和结尾的空格
  trimmed_result=$(echo "$original_string" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

  # 输出修正后的结果
  echo "$trimmed_result"
}

function create_rules(){
    yq  '.[] | [.dev_path, .ln_name]' "$SCRIPT_DIR/create_rule_files.yaml" | while read -r dev_path;read -r ln_name
    if [ $? -ne 0 ]; then
      return 1
    fi

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

      SUBSYSTEM=$(udevadm info --attribute-walk --name=$dev_path  |grep 'SUBSYSTEM==' -m 1)
      SUBSYSTEMS=$(udevadm info --attribute-walk --name=$dev_path  |grep 'SUBSYSTEMS=='  -m 1)
      KERNELS=$(udevadm info --attribute-walk --name=$dev_path  |grep 'KERNELS==' -m 1)
      ATTRS_idVendor=$(udevadm info --attribute-walk --name=$dev_path  |grep 'ATTRS{idVendor}=='  -m 1)
      ATTRS_idProduct=$(udevadm info --attribute-walk --name=$dev_path  |grep 'ATTRS{idProduct}=='  -m 1)
      ATTR_name=$(udevadm info --attribute-walk --name=$dev_path  |grep 'ATTR{name}=='  -m 1)

      SUBSYSTEM=$(fix_space_string "$SUBSYSTEM")
      SUBSYSTEMS=$(fix_space_string "$SUBSYSTEMS")
      KERNELS=$(fix_space_string "$KERNELS")
      ATTRS_idVendor=$(fix_space_string "$ATTRS_idVendor")
      ATTRS_idProduct=$(fix_space_string "$ATTRS_idProduct")
      ATTR_name=$(fix_space_string "$ATTR_name")

      # $ATTRS_idProduct, $ATTRS_idVendor, 
      echo "$SUBSYSTEM, $KERNELS, $SUBSYSTEMS, $ATTR_name, SYMLINK+=\"$SYMLINK\""
      
      if [ -z "$SUBSYSTEM" ] && [ -z "$SUBSYSTEMS" ]; then
        return 1
      fi
    done

    return 0
}

echo -----------------------------------------------------------------------------------------------------------------------
create_rules
if [ $? -eq 0 ]; then
  log_info "规则文件生成成功"
  echo -----------------------------------------------------------------------------------------------------------------------
  log_info "cmd > \nsudo vim /etc/udev/rules.d/99-webcam.rules \nsudo udevadm control --reload-rules \nsudo udevadm trigger\n"
else
  log_error "规则文件生成失败"
fi

