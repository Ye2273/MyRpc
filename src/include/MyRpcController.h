#pragma once
#include <google/protobuf/service.h>
#include <string>


class MyRpcController : public google::protobuf::RpcController
{
private:
    /* data */
public:
    MyRpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string &reason);

    // 未实现的接口
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);
private:
    // rpc调用是否失败
    bool failed_;
    // rpc调用失败原因
    std::string reason_;

};

