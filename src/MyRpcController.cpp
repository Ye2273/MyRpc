#include "MyRpcController.h"



MyRpcController::MyRpcController()
{
    failed_ = false;
    reason_ = "";
}
void MyRpcController::Reset()
{
    failed_ = false;
    reason_ = "";
}
bool MyRpcController::Failed() const
{
    return failed_;
}
std::string MyRpcController::ErrorText() const
{
    return reason_;
}
void MyRpcController::SetFailed(const std::string &reason)
{
    failed_ = true;
    reason_ = reason;
}
// 未实现的接口
void MyRpcController::StartCancel()
{

}
bool MyRpcController::IsCanceled() const
{
    return false;
}
void MyRpcController::NotifyOnCancel(google::protobuf::Closure *callback){}

