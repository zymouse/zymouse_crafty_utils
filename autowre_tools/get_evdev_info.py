#!/usr/bin/python3

from evdev import InputDevice, list_devices, ecodes, categorize

devices = [InputDevice(path) for path in list_devices()]
dev_input_path = ''
for device in devices:
    if ecodes.EV_KEY in device.capabilities() and device.name=='HID 2704:2018' :
        # print(f"device_name:{dev_input_path}\ndevice_phys:{device.phys}]\n")
        print(f"device_path:{device.path}")
        print(f"device_name:{device.name}")
        print(f"device_phys:{device.phys}\n")
