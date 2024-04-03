# tailscale linux 便捷启动

## 如何使用

### 运行tailscale
> 国外使用
```shell
curl -fsSL https://raw.githubusercontent.com/zymouse/zymouse_crafty_utils/main/teleworking/tailscale/tailscale.sh | bash -s -- <token-key>
```
> 国内使用
```shell
curl -fsSL https://gitee.com/zymouse/zymouse_crafty_utils/blob/master/tailscale.sh | bash -s -- <token-key>
```
![](./success.png)

### 运行tailscale 代理功能
> 国外使用
```shell
curl -fsSL https://raw.githubusercontent.com/zymouse/zymouse_crafty_utils/main/teleworking/tailscale/tailscale_exit_node.sh | bash -s -- <exit-node-ip>
```
> 国内使用
```shell
curl -fsSL https://gitee.com/zymouse/zymouse_crafty_utils/blob/master/tailscale_exit_node.sh | bash -s -- <exit-node-ip>
```
