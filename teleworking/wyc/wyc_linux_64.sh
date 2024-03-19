#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

token=$1
$SCRIPT_DIR/wyc_linux_64 -token $token