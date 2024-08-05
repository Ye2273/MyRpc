#include "MyRpcProvider.h"
#include "MyRpcApplication.h"
#include "MyRpcMessage.pb.h"
#include "MyRpcLogger.h"
#include "MyRpcZooKeeper.h"

using namespace std::placeholders;

// 框架提供给外部使用的，用于发布rpc方法的函数
void MyRpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo serviceInfo;

    // service对象中包含了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *desc = service->GetDescriptor();
    // 服务对象的名字
    std::string serviceName = desc->name();
    // 服务对象中包含了服务对象的方法的描述信息
    for (int i = 0; i < desc->method_count(); ++i)
    {
        // 获取服务对象的方法的描述信息,通过索引获取
        const google::protobuf::MethodDescriptor *methodDesc = desc->method(i);
        // 方法的名字
        std::string methodName = methodDesc->name();
        // 方法的描述信息
        serviceInfo.methodMap.insert({methodName, methodDesc});
    }
    serviceInfo.service = service;
    // 将服务对象的名字和服务对象的方法的描述信息保存到map中
    m_serviceMap.insert({serviceName, serviceInfo});
}

// 启动rpc服务节点，开始提供rpc远程网络调用服务
void MyRpcProvider::Run()
{
    // 分离 网络层 和 业务层
    // 从配置文件中读取rpcserver的ip和端口号
    std::string ip = MyRpcApplication::Instance()->GetConf().Load("rpcserverip");
    uint16_t port = std::stoi(MyRpcApplication::Instance()->GetConf().Load("rpcserverport"));
    // 创建一个inet地址
    muduo::net::InetAddress addr(ip, port, false);
    // 1 创建一个tcp服务对象
    muduo::net::TcpServer server(&loop_, addr, "MyRpcProvider");
    // 2 绑定连接回调     
    server.setConnectionCallback(std::bind(&MyRpcProvider::onConnection, this, _1));
    // 3 绑定消息读写回调
    server.setMessageCallback(std::bind(&MyRpcProvider::onMessage, this, _1, _2, _3));
    // 4 设置muduo库的线程数量
    server.setThreadNum(4);

    // 启动zookeeper,将rpc服务提供者的地址信息注册到zookeeper中，供rpc服务消费者发现
    MyRpcZooKeeper::GetZooKeeper()->Start();
    for(auto &sp : m_serviceMap)
    {
        // 服务名字
        std::string serviceName = "/"+sp.first;
        // 创建一个服务节点,服务节点是持久节点
        MyRpcZooKeeper::GetZooKeeper()->CreateNode(serviceName, "", 0, 0);
        // 创建服务节点下的方法节点
        for(auto &mp : sp.second.methodMap)
        {
            // 方法名字
            std::string methodName = serviceName + "/" + mp.first;
            
            char methodPath[128] = {0};
            // 方法节点的值是ip:port
            sprintf(methodPath, "%s:%d", ip.c_str(), port);
            // 创建方法节点,方法节点是临时节点
            MyRpcZooKeeper::GetZooKeeper()->CreateNode(methodName, methodPath, strlen(methodPath), ZOO_EPHEMERAL);
        }
    }
    std::cout << "rpc server start at " << ip << ":" << port << std::endl;

    // 5 启动网络服务
    server.start();
    // 6 启动事件循环(等待事件的发生)
    loop_.loop();
}

// 连接回调，如果有远端连接过来，这个函数会被调用
void MyRpcProvider::onConnection(const muduo::net::TcpConnectionPtr&conn)
{
    if(!conn->connected())
    {
        // 客户端断开连接
        conn->shutdown();
        LOG_INFO("client disconnected!");
    }
}
/* 
    protobuf的消息格式(MyRpcProvider和MyRpcConsumer之间的通信)
            UserServiceLoginYe shen123456
*/
// 消息读写回调, 如果有rpc调用请求消息过来，这个函数会被调用
void MyRpcProvider::onMessage(const muduo::net::TcpConnectionPtr&conn, muduo::net::Buffer*buffer , muduo::Timestamp)
{
    // 网络上接收远程rpc调用请求的字符流
    std::string recvBuf = buffer->retrieveAllAsString();
    
    // 字符流前面4个字节是长度信息
    uint32_t header;
    recvBuf.copy((char*)&header, 4, 0);
    
    // 原始字符流中的数据,得到序列化的rpc消息
    std::string recvStr = recvBuf.substr(4, header);
    // 反序列化
    MyRpc::MyRpcMessage rpcRequest; 
    rpcRequest.ParseFromString(recvStr);
    // rpc调用请求的服务名字
    std::string serviceName = rpcRequest.service_name();
    // rpc调用请求的方法名字
    std::string methodName = rpcRequest.method_name();
    // rpc调用请求的参数大小
    std::uint32_t argsSize = rpcRequest.args_size();
    // rpc调用请求的参数
    std::string args = recvBuf.substr(4 + header, argsSize);

    // 根据rpc调用请求的服务名字在m_serviceMap中查找对应的服务对象
    auto it = m_serviceMap.find(serviceName);
    if(it == m_serviceMap.end())
    {
        // std::cout << "rpc service: " << serviceName << " not found!" << std::endl;
        LOG_ERROR("rpc service: %s not found!", serviceName.c_str());
        return;
    }
    // 方法描述信息
    auto methodIt = it->second.methodMap.find(methodName);
    if(methodIt == it->second.methodMap.end())
    {
        // std::cout << "rpc method: " << methodName << " not found!" << std::endl;
        LOG_ERROR("rpc method: %s not found!", methodName.c_str());
        return;
    }

    // 服务对象
    google::protobuf::Service *service = it->second.service;
    // 方法method 对象
    const google::protobuf::MethodDescriptor *methodDesc = methodIt->second;
    // 生成rpc方法调用的请求request，这个 request 对象是一个 protobuf 消息，用于存储反序列化后的请求参数。
    google::protobuf::Message *request = service->GetRequestPrototype(methodDesc).New();
    // 反序列化rpc方法调用的请求request，将rpc调用请求的参数args反序列化到request对象中，将 args 中的内容解析为 protobuf 消息对象
    request->ParseFromString(args);

    // 生成rpc方法调用的响应response
    google::protobuf::Message *response = service->GetResponsePrototype(methodDesc).New();

    // 给下面的CallMethod方法传递一个回调对象，绑定Closure回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback<MyRpcProvider, 
                                                                const muduo::net::TcpConnectionPtr&, 
                                                                google::protobuf::Message &>
                                                                (this, &MyRpcProvider::sendRpcResponse, conn, *response);

    // 根据rpc请求调用的方法method对象，调用服务对象的方法，执行rpc方法调用, 调用到了服务器自定义的业务逻辑
    // e.g: UserService.Login(ctx, request, response, callback)
    service->CallMethod(methodDesc, nullptr, request, response, done);



}

// Closure回调操作，rpc 响应（序列化和网络发送）
void MyRpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr&conn, google::protobuf::Message &response)
{
    std::string responseStr;
    // 序列化rpc方法调用的响应response
    if(response.SerializeToString(&responseStr))
    {
        // 序列化成功,通过网络发送给rpc调用方
        conn->send(responseStr);
        LOG_INFO("send response success!");
        // 模拟http的短连接，由服务器主动断开连接
        conn->shutdown();

    }
    else
    {
        // std::cout << "serialize response error!" << std::endl;
        LOG_ERROR("serialize response error!");
    }
}


