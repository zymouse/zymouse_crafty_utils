#!/usr/bin/python3
"""
获取工控机IP,显示到桌面
"""
import tkinter as tk
import netifaces as ni


class IPMonitor(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("IP Monitor")
        self.configure(bg="gray17")
        self.attributes('-topmost', 1)
        self.overrideredirect(1)  # 无边框

        self.ip_container = tk.Frame(self, bg="gray17")
        self.ip_container.pack(pady=20, padx=20, fill=tk.BOTH, expand=True)

        # 添加鼠标拖动功能
        self.bind('<Button-1>', self.start_move)
        self.bind('<B1-Motion>', self.stop_move)

        self.ip_addresses = {}
        self.update_ip_display()

    def start_move(self, event):
        self.x = event.x
        self.y = event.y

    def stop_move(self, event):
        x = (event.x_root - self.x - self.winfo_x())
        y = (event.y_root - self.y - self.winfo_y())
        self.geometry("+%s+%s" % (x, y))

    def on_closing(self):
        self.destroy()

    def get_all_ip_addresses(self):
        addresses = {}
        interfaces = ni.interfaces()
        for interface in interfaces:
            try:
                ip = ni.ifaddresses(interface)[ni.AF_INET][0]['addr']
                addresses[interface] = ip
            except (ValueError, KeyError, IndexError):
                pass
        return addresses

    def update_ip_display(self):
        new_addresses = self.get_all_ip_addresses()
        
        # Only update if the IP addresses have changed.
        if new_addresses != self.ip_addresses:
            self.ip_addresses = new_addresses
            for widget in self.ip_container.winfo_children():
                widget.destroy()

            # Create a row frame for the first IP and the close button
            row_frame = tk.Frame(self.ip_container, bg="gray17")
            row_frame.pack(fill=tk.X, expand=True, pady=2)
            
            if self.ip_addresses:
                first_key = next(iter(self.ip_addresses))
                lbl = tk.Label(row_frame, text=f"{first_key}: {self.ip_addresses[first_key]}", fg="white", bg="gray17")
                lbl.pack(side=tk.LEFT, expand=True)
                del self.ip_addresses[first_key]

            self.close_button = tk.Button(row_frame, text="X", command=self.on_closing, bg="red", fg="white", bd=0)
            self.close_button.pack(side=tk.RIGHT)

            for k, v in self.ip_addresses.items():
                lbl = tk.Label(self.ip_container, text=f"{k}: {v}", fg="white", bg="gray17")
                lbl.pack(anchor=tk.W, pady=2)

            # Adjust window size to fit content
            self.update_idletasks()  # Update the window
            screen_width = self.winfo_screenwidth()
            screen_height = self.winfo_screenheight()
            window_width = self.winfo_width()
            window_height = self.winfo_height()
            position_x = int(screen_width / 2 - window_width / 2)
            position_y = screen_height - window_height
            self.geometry(f"{window_width}x{window_height}+{position_x}+{position_y}")

        # Check again after a second.
        self.after(1000, self.update_ip_display)


if __name__ == "__main__":
    app = IPMonitor()
    app.mainloop()
