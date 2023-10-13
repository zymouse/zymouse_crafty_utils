#!/usr/bin/python3
"""
获取工控机IP,显示到桌面
"""
import netifaces as ni
import tkinter as tk

def get_all_ip_addresses():
    ip_addresses = []
    # Get a list of all available network interfaces
    interfaces = ni.interfaces()

    for interface in interfaces:
        try:
            ip = ni.ifaddresses(interface)[ni.AF_INET][0]['addr']
            if not ip.startswith('127.'):  # Ignore loopback addresses like 127.0.0.1
                ip_addresses.append(f"{interface}: {ip}")
        except:
            continue

    if not ip_addresses:
        raise ValueError("No available network interface found.")
    
    return ip_addresses

def create_transparent_window_with_ips():
    ip_addresses = get_all_ip_addresses()

    root = tk.Tk()
    root.title("IP Addresses")

    # Window dimensions
    window_width = 300
    window_height = 30 * len(ip_addresses) + 30  # height is determined by number of IP addresses

    # Position the window at the bottom center of the screen
    screen_width = root.winfo_screenwidth()
    x_pos = (screen_width // 2) - (window_width // 2)
    y_pos = root.winfo_screenheight() - window_height - 10

    root.geometry(f"{window_width}x{window_height}+{x_pos}+{y_pos}")
    root.overrideredirect(1)
    root.attributes('-alpha', 0.0)
    root.attributes("-topmost", True)

    for ip in ip_addresses:
        lbl = tk.Label(root, text=ip, font=("Arial", 12), bg='black', fg='white')
        lbl.pack(pady=2)

    close_button = tk.Button(root, text="Close", command=root.destroy, bg='red', fg='white')
    close_button.pack(pady=5)

    root.mainloop()

try:
    create_transparent_window_with_ips()
except KeyboardInterrupt:
    print("\nProgram terminated by user.")
