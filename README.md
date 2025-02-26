# 项目介绍

## 这是一个仿造新版QQ制作的聊天服务器+客户端解决方案

## 包含内容

### 服务器端
* **Status Server** 负载均衡服务器，为客户端选择一个服务端进行分配，目前的策略是取余选择

* **Chat Server** 聊天服务器，负责与客户端进行通信

* **VarifyServer** NODE JS的邮件发送服务器，用ASIO做连接池

* **WebServer** 以Crow作为提供由代码打包二进制压缩文件的高速流式下载和密码重置功能


# 客户端
* **ChatClient** 基于新版QQ的客户端，使用Qt进行开发


# 引用
本项目部分服务器端的风格和邮件发送源码参考了llfchat的设计，详见项目地址：https://gitee.com/secondtonone1/llfcchat