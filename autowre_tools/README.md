# 使用方法

## keyboard_publisher - 硬件按钮使能autoware

## 1.0 安装
```shell
python3 -m pip install evdev

```

## 使用方法
### step-1：获取设备信息并填写配置参数
```shell
./get_evdev_info.py 
    device_path:/dev/input/event13
    device_name:HID 2704:2018
    device_phys:usb-0000:00:14.0-8/input1

    device_path:/dev/input/event12
    device_name:HID 2704:2018
    device_phys:usb-0000:00:14.0-8/input0
```
把`/input0`结尾到填写配置文件
比如：`device_name:HID 2704:2018`和`device_phys:usb-0000:00:14.0-8/input0`填写到参数文件`keyboard_publisher.yaml`

### step-2：赋予权限
> `/dev/input/event12`根据上一步得到

```shell
sudo chmod 777 /dev/input/event12
```

### step-2：运行脚本
```shell
source autoware_robobus_install/setup.bash
./keyboard_publisher.py
```