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

map_path=$1
if [ -z "$map_path" ]; then
    log_error '请输入pointcloud_divider.sh [map_folder_name]'
    exit 1
fi

pcd_name=pointcloud_map
map_path=$SCRIPT_DIR/$map_path
if [ -f "$map_path/$pcd_name.pcd" ]; then
    log_info "在$map_path 中找到$pcd_name.pcd文件"
else
    log_error "文件$pcd_name.pcd不存在于-$map_path中。"
    exit 1
fi

log_info "创建文: $map_path/metadata.yaml"
cat << EOF > "$map_path/metadata.yaml"
pointcloud_divider:
  use_large_grid: false
  merge_pcds: false
  leaf_size: 0.0 # downsample leaf size
  grid_size_x: 20.0 # grid size of x
  grid_size_y: 20.0 # grid size of y
EOF

log_info "开始分片"
# rm -r $map_path/$pcd_name.pcd/pointcloud_map_metadata.yaml
# if [ -d "$map_path/$pcd_name.pcd" ]; then
#     rm -r $map_path/$pcd_name.pcd
# fi
# mv $map_path/$pcd_name.pcd.2 $map_path/$pcd_name.pcd


in_map_path=$map_path
out_map_path=$map_path/$pcd_name.pcd.1
mkdir $out_map_path
$SCRIPT_DIR/pointcloud_divider/scripts/pointcloud_divider.sh \
    $in_map_path \
    $out_map_path \
    $pcd_name \
    $map_path/metadata.yaml

if [ $? -eq 0 ]; then
    log_info "分片成功"
else
    log_error "分片失败"
    exit 1
fi


log_info "替换文件：$pcd_name.pcd-->$pcd_name.pcd.2, 文件夹：$pcd_name.pcd.1-->$pcd_name.pcd"
mv $map_path/$pcd_name.pcd $map_path/$pcd_name.pcd.2
mv $map_path/$pcd_name.pcd.1  $map_path/$pcd_name.pcd
mv $map_path/$pcd_name.pcd/pointcloud_map_metadata.yaml $map_path/