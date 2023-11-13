# zymouse_crafty_utils

## 目录
- [OSM文件转换器](./gen_OSM_cloud/README.md)
- [ROS2数据采集脚本](./collection_ros2bag/README.md)
- [超声波can数据分析](./ultra_sonic_test/README.md)
- [IP修改器-livox](./sensor_tools/livox/README.md)
- [IP修改器-ouster](./sensor_tools/ouster/README.md)
- [工作效率脚本](./script/README.md)
- [autoware按钮遥控](./autowre_tools/README.md)

## 介绍
- `OSM文件转换器`:通过[tier4绘图工具](https://tools.tier4.jp/feature/vector_map_builder_ll2/)制作OSM地图文件，在该地图文件添加经纬度，按照OSM标准修改字段等操作。修改后的OSM地图文件通过[geojson工具](https://github.com/tyrasd/osmtogeojson)转换成地图json文件，该json地图文件可以在[高德](https://lbs.amap.com/demo/javascript-api/example/overlayers/geojson)等地图厂商可视化显示
- `ROS2数据采集脚本`: 采集配置文件话题列表ROS2BAG包，可以分析话题是否存在
- `超声波can数据分析`: 录制can数据，并可视化分析数据
- `IP修改器`:  终端执行修改程序即可修改Lidar的IP，使得修改变得方便快捷
- `autoware按钮遥控`: 通过点击按钮实现autowre的engage和速度下发



