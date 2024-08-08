#include <iostream>
#include <string>
#include "MyRpcApplication.h"
#include "UserProto.pb.h"
#include "MyRpcChannel.h"


int main(int argc, char **argv)
{
    // 1 初始化应用层
    MyRpcApplication::Instance()->Init(argc, argv);
    // 2 获取UserServiceRpc_Stub对象
    example::UserServiceRpc_Stub stub(new MyRpcChannel());
    // 3 构造请求和响应对象
    example::LoginRequest request;
    example::LoginResponse response;
    // 4 设置请求对象的数据
    request.set_username("Ye sheng");
    request.set_password("123456");

    // 5 发送rpc方法调用,同步调用，阻塞等待返回结果
    stub.login(nullptr, &request, &response, nullptr);

    // 6 打印响应结果
    if(response.result().code() == 0)
    {
        std::cout << "login success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "login failed: " << response.result().message() << std::endl;
    }
    return 0;
}