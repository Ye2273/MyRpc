# MyRpc分布式框架
> 关于项目的笔记：https://blog.csdn.net/weixin_67134938/article/details/140951712
## 安装使用

> 环境：Ubuntu 20.0.1、C++
>
> 将 lib 中的静态库文件 libMyRpc.a  放在 usr/lib  或者 usr/local/lib
>
>  include中所有头文件 放在 usr/include 或者 usr/local/include

这样就能实现默认调用。

## 简介

### 什么是RPC

**远程过程调用（Remote Procedure Call, RPC）** 是一种计算机通信协议，它允许一个程序在不同的地址空间（通常是另一台机器上）上执行某个子程序或过程，就像是调用本地子程序一样。RPC 将网络通信的复杂性隐藏在简单的函数调用接口之后，使得分布式计算变得更容易。

### 为什么要使用RPC

1. 透明性：
	- RPC 隐藏了底层网络通信的细节，开发者只需关注业务逻辑，就像调用本地方法一样调用远程服务。
2. 简化分布式计算：
	- 通过 RPC，可以将不同的功能模块分布到不同的服务器上运行，实现负载均衡和系统扩展性。
3. 语言无关性：
	- 很多 RPC 框架支持多种编程语言，可以使不同语言编写的程序之间进行通信。
4. 性能优化：
	- 通过使用高效的二进制协议进行数据传输，可以大幅提升性能。

> 基于此开发一款轻量级的RPC框架：

## MyRpc框架

![image-20240805202945851](https://github.com/Ye2273/MyRpc/blob/master/png/image-20240805202945851.png)

黄色部分：设计rpc方法参数的打包和解析，也就是数据的序列化和反序列化，使用Protobuf。

绿色部分：网络部分，包括寻找rpc服务主机，发起rpc调用请求和响应rpc调用结果，使用muduo网络库和zookeeper服务配置中心（专门做服务发现）。



## 整个框架运作过程

<img src="https://github.com/Ye2273/MyRpc/blob/master/png/image-20240805203909945.png" alt="image-20240805203909945" style="zoom: 80%;" />

> **仅作学习之用~**
