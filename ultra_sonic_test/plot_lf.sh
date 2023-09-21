#!/bin/bash
candump_data_file=$1
if [ -z "$candump_data_file" ];then
        log_error "请输入[ ./plot_lf.sh ultra_sonic_test.log]"
        exit 1
fi
filterate1_4='Ultra5To8.Ultra5:-< Ultra5To8.Ultra6:-<  Ultra9To12.Ultra11:-<  Ultra9To12.Ultra12:-<'
cat $candump_data_file | python3 -m cantools plot ultra_sonic_radar.dbc $filterate1_4 