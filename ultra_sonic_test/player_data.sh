#!/bin/bash
candump_data_file=$1
if [ -z "$candump_data_file" ];then
        log_error "请输入[ ./player_data.sh ultra_sonic_test.log]"
        exit 1
fi

canplayer -I $candump_data_file