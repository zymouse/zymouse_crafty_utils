#!/usr/bin/env python3
import time
import signal
import subprocess
import platform
import os

# 输出文件
# 获取系统架构
arch = platform.machine().lower()
if "arm" in arch or "aarch64" in arch:
    LOG_FILE = "/home/nvidia/pix/ros2bag/time_jump.log"
elif "x86_64" in arch or "amd64" in arch:
    LOG_FILE = "/home/ipc/pix/ros2bag/time_jump.log"
else:
    LOG_FILE = "/tmp/time_jump.log"  # 默认路径
# 确保目录存在
os.makedirs(os.path.dirname(LOG_FILE), exist_ok=True)
print("当前架构:", arch)
print("日志路径:", LOG_FILE)


# 跳变阈值（秒）
THRESHOLD = 0.001

# 退出标志
running = True


def signal_handler(sig, frame):
    """捕获 Ctrl+C 信号，优雅退出"""
    global running
    print("\n[INFO] 捕获 Ctrl+C，正在退出...")
    running = False

def get_time_source():
    try:
        result = subprocess.check_output(
            "chronyc sources | awk '/\\*/ {print $2}'",
            shell=True,
            text=True
        ).strip()
        return result if result else "Unknown"
    except Exception as e:
        return f"Error: {e}"


def main():
    # 绑定 Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)

    base_realtime = time.time()
    base_monotonic = time.monotonic()

    last_diff = None

    with open(LOG_FILE, "a") as f:
        f.write(f"\n--- 启动监控 {time.strftime('%Y-%m-%d %H:%M:%S')} ---\n")

        while running:
            now_realtime = time.time()
            now_monotonic = time.monotonic()

            diff = (now_realtime - base_realtime) - (now_monotonic - base_monotonic)

            # 如果首次运行 或者 差值变化超过阈值，写日志
            if last_diff is None or abs(diff - last_diff) > THRESHOLD:
                time_source = get_time_source()
                log_line = f"{time.strftime('%Y-%m-%d %H:%M:%S')} diff={diff:.6f}s, 时间源:{time_source}\n"
                f.write(log_line)
                f.flush()  # 立即写盘
                print(log_line, end="")

                last_diff = diff

            time.sleep(1)


if __name__ == "__main__":
    main()
