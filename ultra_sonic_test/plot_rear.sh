#!/bin/bash
candump_data_file=$1
if [ -z "$candump_data_file" ];then
        log_error "请输入[ ./plot_rear.sh ultra_sonic_test.log]"
        exit 1
fi
filterate7_10='Ultra5To8.Ultra7:-> Ultra5To8.Ultra8:->  Ultra9To12.Ultra9:-> Ultra9To12.Ultra10:->'
cat $candump_data_file | python3 -m cantools plot ultra_sonic_radar.dbc  $filterate7_10