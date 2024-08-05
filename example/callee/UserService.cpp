#include <iostream>
#include <string>
#include "MyRpcApplication.h"
#include "UserProto.pb.h"
#include "MyRpcProvider.h"
#include "MyRpcLogger.h"
class UserService : public example:: UserServiceRpc{            // 1 rpc接口继承自生成的UserServiceRpc，想在网络提供rpc服务，需要继承自生成的rpc接口
public:
    // 微服务，通过rpc实现将login方法进行单独部署，不跟其他业务耦合
    bool Login(const std::string& username, const std::string& password) {
        std::cout << "Login: " << username << ", " << password << std::endl;
        return true;
    }
    /*
        2 重写rpc接口的方法, 该方法会在网络调用时被调用,框架会自动调用
            调用方的 rpc请求通过网络发送过来，被框架接收到，然后调用该方法，该方法中可以调用Login方法，实现rpc调用，然后将结果返回给框架，框架再将结果返回给客户端，完成rpc调用
            controller: rpc调用的控制器，可以设置超时时间，取消rpc调用等
            request: rpc调用的请求参数
            response: rpc调用的响应参数
            done: rpc调用完成后的回调函数，可以在回调函数中释放资源等
    */ 
    void login(::google::protobuf::RpcController* controller,
                       const ::example::LoginRequest* request,
                       ::example::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {       
        // 3 获取rpc调用的请求参数
        const std::string& username = request->username();
        const std::string& password = request->password();
        // 4 调用Login方法
        bool ret = Login(username, password);
        // 5 设置rpc调用的响应参数
        example::Result* result = response->mutable_result();
        result->set_code(0);
        result->set_message("success");
        response->set_success(ret);
        // 6 rpc调用完成后的回调函数    序列化响应参数，网络发送功能（框架实现）
        done->Run();
    }
};


int main(int argc, char **argv) {

    // 7 调用框架的初始化，配置rpc服务
    MyRpcApplication::Instance()->Init(argc, argv);

    // 8 创建rpc服务提供者对象，专门用于将UserService对象发布到rpc节点中 （网络）
    MyRpcProvider rpcProvider;        // 是网络服务对象

    // 9 启动rpc服务提供者，发布rpc方法
    rpcProvider.NotifyService(new UserService());   // 要提供其他方法，只需要继承UserServiceRpc，然后实现对应的方法，NotifyService可以多次调用，发布多个rpc服务

    LOG_INFO("UserService start...");    // 11 记录日志

    // 10 启动一个rpc服务，run方法是阻塞的，等待远程rpc调用请求
    rpcProvider.Run();


    return 0;   
}