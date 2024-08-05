#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders; // 占位符

/*基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop事件循环对象的指针
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写时间的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
*/

class TestServer {
    public:
        TestServer(EventLoop* loop, const InetAddress& listenAddr, const string& name)
            : server_(loop, listenAddr, name) {
                server_.setConnectionCallback(std::bind(&TestServer::onConnection, this, _1));
                server_.setMessageCallback(std::bind(&TestServer::onMessage, this, _1, _2, _3));
            }

        void start() {
            server_.start();
        }

    private:
        TcpServer server_;
        EventLoop* loop;

        void onConnection(const TcpConnectionPtr& conn) {
            if (conn->connected()) {
                std::cout << "new connection: " << conn->peerAddress().toIpPort() << std::endl;
            } else {
                std::cout << "connection is down: " << conn->peerAddress().toIpPort() << std::endl;
            }
        }

        void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
            std::string buf = buffer->retrieveAllAsString();
            std::cout << "recv data: " << buf << "time: " << time.toString() << std::endl;
            conn->send(buf);
        }
        
};


int main () {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 3000);
    TestServer server(&loop, addr, "TestServer");
    server.start();
    loop.loop();
    return 0;
}
