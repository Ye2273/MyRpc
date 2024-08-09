#include "MyRpcChannel.h"
#include "MyRpcLogger.h"
#include "MyRpcZooKeeper.h"
// 所有通过stub调用的rpc方法，最终都会执行到这里，统一做rpc方法调用的数据序列化和网络发送
void MyRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                google::protobuf::RpcController *controller,
                const google::protobuf::Message *request,
                google::protobuf::Message *response,
                google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    // 1. 获取服务名字
    const std::string &service_name = sd->name();
    // 2. 获取方法名字
    const std::string &method_name = method->name();
    // 3. 序列化请求参数
    std::string args_str;
    // 4. 获取参数序列化后的大小
    uint32_t argsSize = 0;
    if(request->SerializeToString(&args_str))
    {
        argsSize = args_str.size();
    }
    else
    {
        // std::cout << "serialize request error!" << std::endl;
        LOG_ERROR("serialize request error!");
    }

    // 5. 设置rpc方法调用请求数据
    MyRpc::MyRpcMessage rpcRequest;
    rpcRequest.set_service_name(service_name);
    rpcRequest.set_method_name(method_name);
    rpcRequest.set_args_size(argsSize);

    // 6. 将rpc方法调用请求数据序列化
    std::string rpcRequest_str;
    // 7. 获取rpc方法调用请求数据序列化后的大小
    uint32_t rpcRequest_size = 0;
    if(rpcRequest.SerializeToString(&rpcRequest_str))
    {
        rpcRequest_size = rpcRequest_str.size();
    }
    else
    {
        // std::cout << "serialize rpcRequest error!" << std::endl;
        // LOG_ERROR("serialize rpcRequest error!");
        controller->SetFailed("serialize rpcRequest error!");
        return;
    }

    // 8. 组装待发送的rpc请求字符串
    std::string send_str;
    send_str.append((char *)&rpcRequest_size, 4);
    send_str.append(rpcRequest_str);
    send_str.append(args_str);

    // 9. 发送rpc请求数据，因为是客户端不用高并发，所以用tcp连接发送
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientFd == -1)
    {
        // std::cout << "create socket error!" << std::endl;
        // LOG_ERROR("create rpc request socket error!");
        controller->SetFailed("create rpc request socket error!");
        return;
    }
    // // 10. 从配置文件中读取rpcserver的ip和端口号
    // std::string ip = MyRpcApplication::Instance()->GetConf().Load("rpcserverip");
    // uint16_t port = std::stoi(MyRpcApplication::Instance()->GetConf().Load("rpcserverport"));

    // 10. 从zoopkeeper中获取rpcserver的ip和端口号
    MyRpcZooKeeper::GetZooKeeper()->Start();
    std::string methodPath = "/" + service_name + "/" + method_name;
    std::string hostData = MyRpcZooKeeper::GetZooKeeper()->GetNodeData(methodPath);
    if(hostData == "")
    {
        // std::cout << "get rpc server ip and port error!" << std::endl;
        // LOG_ERROR("get rpc server ip and port error!");
        controller->SetFailed("get rpc server ip and port error!");
        close(clientFd);
        return;
    }
    int pos = hostData.find(":");
    std::string ip = hostData.substr(0, pos);
    uint16_t port = std::stoi(hostData.substr(pos + 1));


    // 11. 创建一个inet地址
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    
    // 12. 连接rpcserver
    if (connect(clientFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        // std::cout << "connect error!" << std::endl;
        // LOG_ERROR("rpc request connect error!");
        controller->SetFailed("rpc request connect error!");
        close(clientFd);
        return;
    }
    // 13. 发送rpc请求数据
    if(send(clientFd, send_str.c_str(), send_str.size(), 0) == -1)
    {
        // std::cout << "send error!" << std::endl;
        // LOG_ERROR("send rpc request error!");
        controller->SetFailed("send rpc request error!");
        close(clientFd);
        return;
    }
    
    // 14. 接收rpc响应数据
    char recvBuf[1024] = {0};
    int recvLen = recv(clientFd, recvBuf, 1024, 0);
    if(recvLen == -1)
    {
        // std::cout << "recv error!" << std::endl;
        // LOG_ERROR("recv rpc response error!");
        controller->SetFailed("recv rpc response error!");
        close(clientFd);
        return;
    }
    // 15. 转成string 
    std::string response_str(recvBuf, 0, recvLen);
    // 16. 反序列化rpc响应数据,填充到response对象中
    if(!response->ParseFromString(response_str))
    {
        // std::cout << "parse response error!" << std::endl;
        // LOG_ERROR("parse rpc response error!");
        controller->SetFailed("parse rpc response error!");
        close(clientFd);
        return;
    }
    // 17. 关闭连接
    close(clientFd);
    // 18. 执行回调
    // done->Run();

}