#!/usr/bin/python3
from evdev import InputDevice, categorize, ecodes, list_devices
import threading
import yaml
import select
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile

from autoware_auto_vehicle_msgs.msg import Engage
from tier4_planning_msgs.msg import VelocityLimit

def ends_with_same_substring(strA, strB):
    min_length = min(len(strA), len(strB))
    return strA[-min_length:] == strB[-min_length:]

class KeyboardPublisher(Node):
    def __init__(self):
        super().__init__('keyboard_publisher')
        self.autoware_engage_pub = self.create_publisher(Engage, "/autoware/engage", QoSProfile(depth=5))
        self.vehicle_velocity_pub = self.create_publisher(VelocityLimit, "/planning/scenario_planning/max_velocity", QoSProfile(depth=5))
        self.autoware_engage_pub_bool = True
        self.yaml_data = {}
        
        self.dev_input_path = self.find_keyboard_device()
        if self.dev_input_path is None:
            raise RuntimeError("键盘设备未找到或配置文件错误")
        self.shutdown_called = False
        self.exit_flag = threading.Event()
        self.keyboard_thread = threading.Thread(target=self.keyboard_event_listener)
        self.keyboard_thread.start()

    def pub_velocity_limit(self, kmps:float):
        velocity_limit = VelocityLimit()
        velocity_limit.stamp = self.get_clock().now().to_msg()
        velocity_limit.max_velocity = kmps*0.2777; # m/s
        velocity_limit.use_constraints = True
        velocity_limit.constraints.min_acceleration = -1.0
        velocity_limit.constraints.min_jerk = -1.0
        velocity_limit.constraints.max_jerk = 1.0
        velocity_limit.sender = ""
        self.vehicle_velocity_pub.publish(velocity_limit)
        
    def pub_engage(self, engage:bool):
        engage_pub_msg = Engage()
        engage_pub_msg.stamp = self.get_clock().now().to_msg()
        engage_pub_msg.engage = engage
        self.autoware_engage_pub.publish(engage_pub_msg)
        
    def keyboard_event_listener(self):
        keyboard = InputDevice(self.dev_input_path)
        while not self.exit_flag.is_set():
            r, w, x = select.select([keyboard.fd], [], [], 0.1)
            if r:
                for event in keyboard.read():
                    if event.type == ecodes.EV_KEY:
                        event_status = 'down' if event.value == 1 else 'up' if event.value == 0 else 'hold'
                        if event.code == 2 and event_status=='down':
                            self.pub_engage(True)
                        elif event.code == 3 and event_status=='down':
                            self.pub_engage(False)
                        elif event.code == 4 and event_status=='down':
                            self.pub_velocity_limit(self.yaml_data['button_C'])
                        elif event.code == 5 and event_status=='down':
                            self.pub_velocity_limit(self.yaml_data['button_D'])
                        else:
                            pass
        print("键盘事件监听器已停止")
    
    def find_keyboard_device(self):
            with open('keyboard_publisher.yaml', 'r') as stream:
                try:
                    self.yaml_data = yaml.safe_load(stream)
                    print(self.yaml_data)  # 打印读取的数据
                except yaml.YAMLError as exc:
                    self.get_logger().error("配置文件错误")
                    return None
            devices = [InputDevice(path) for path in list_devices()]
            for device in devices:
                if ecodes.EV_KEY in device.capabilities() and device.name==self.yaml_data['device_name'] and ends_with_same_substring(device.phys, self.yaml_data['device_phys']):
                    return device.path
            
            self.get_logger().error("请插入遥控器")
            return None

    def stop(self):
        self.exit_flag.set()  # 设置退出标志
        self.keyboard_thread.join()  # 等待线程结束
        if not self.shutdown_called:
            rclpy.shutdown()
            self.shutdown_called = True

def main(args=None):
    rclpy.init(args=args)
    keyboard_publisher = KeyboardPublisher()
    try:
        rclpy.spin(keyboard_publisher)
    except  (KeyboardInterrupt, RuntimeError) as e:
        pass
    finally:
        keyboard_publisher.destroy_node()
        if 'keyboard_publisher' in locals():
            keyboard_publisher.stop()  # 优雅地停止节点和线程
        

if __name__ == '__main__':
    main()
