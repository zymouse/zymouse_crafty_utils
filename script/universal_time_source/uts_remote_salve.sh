sudo apt install linuxptp -y

sudo vim /etc/systemd/system/ptp4l@.service
sudo systemctl daemon-reload
sudo systemctl start ptp4l@eno1.service
sudo systemctl start ptp4l@enp3s0.service
sudo systemctl enable ptp4l@enp3s0.service