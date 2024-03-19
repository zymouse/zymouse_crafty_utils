#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

token=$1
sudo tailscale up --authkey $token