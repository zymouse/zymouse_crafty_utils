#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
curl -fsSL https://tailscale.com/install.sh | sh