#include "MyRpcApplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

void MyRpcApplication::Init(int argc, char **argv)
{
    if (argc < 2) {
        std::cout<<"format: command -i <configfile>" << std::endl;
        exit(0);
    }
    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout<<"format: command -i <configfile>" << std::endl;
            exit(EXIT_FAILURE);
        case ':':
            std::cout<<"format: command -i <configfile>" << std::endl;
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 加载 配置文件    读取配置文件内容，存储到内存中，供框架使用，比如rpc服务提供者的ip和端口号，zookeeper的ip和端口号等
    conf_.LoadFile(config_file);

    // 读取配置文件内容
    // std::string ip = conf_.Load("rpcserverip");
    // std::string port = conf_.Load("rpcserverport");
    // std::cout << "rpcserverip=" << ip << std::endl;
    // std::cout << "rpcserverport=" << port << std::endl;

}

MyRpcApplication* MyRpcApplication::app_ = nullptr;

MyRpcApplication* MyRpcApplication::Instance()
{
    if(app_ == nullptr)
    {
        app_ = new MyRpcApplication();
    }
    return app_;
}
