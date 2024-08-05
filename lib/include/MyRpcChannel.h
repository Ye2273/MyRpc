#pragma once
#include <iostream>
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "MyRpcMessage.pb.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "MyRpcApplication.h"
class MyRpcChannel : public google::protobuf::RpcChannel
{
public:
    // 所有通过stub调用的rpc方法，最终都会执行到这里，统一做rpc方法调用的数据序列化和网络发送
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done);
};