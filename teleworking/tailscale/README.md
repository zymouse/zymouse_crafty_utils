# tailscale linux 便捷启动

## 如何使用

### 运行tailscale
```shell
curl -fsSL https://raw.githubusercontent.com/zymouse/zymouse_crafty_utils/main/teleworking/tailscale/tailscale.sh | bash -s -- <token-key>
```
![](./success.png)

### 运行tailscale 代理功能
```shell
curl -fsSL https://raw.githubusercontent.com/zymouse/zymouse_crafty_utils/main/teleworking/tailscale/tailscale_exit_node.sh | bash -s -- <exit-node-ip>
```
