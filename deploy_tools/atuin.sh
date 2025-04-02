#!/bin/bash

# 不要以root身份运行，如果需要的话，会要求root。
bash <(curl https://raw.githubusercontent.com/ellie/atuin/main/install.sh)

echo 'eval "$(atuin init bash)"' >> ~/.bashrc

atuin import auto
