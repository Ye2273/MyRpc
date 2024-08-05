#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "google/protobuf/service.h"
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>

/* 
    rpc框架中发布rpc服务的提供者(网络服务对象)
 */
class MyRpcProvider
{
public:
    // 框架提供给外部使用的，用于发布rpc方法的函数
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
private:
    // 服务节点EventLoop
    muduo::net::EventLoop loop_;
    // 连接回调
    void onConnection(const muduo::net::TcpConnectionPtr&);

    // 消息读写回调
    void onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp); 

    // Closure回调操作，rpc 响应（序列化和网络发送）
    void sendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message &response);

    // 服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *service; // 服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap; // 方法名字和方法描述信息
    };
    // 服务名字和服务信息（服务对象和方法描述信息）
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
    
};