#include "MyRpcZooKeeper.h"
#include "MyRpcLogger.h"
#include "MyRpcApplication.h"


MyRpcZooKeeper* MyRpcZooKeeper::zooKeeper_ = nullptr;
MyRpcZooKeeper* MyRpcZooKeeper::GetZooKeeper() {
    if(zooKeeper_ == nullptr) {
        zooKeeper_ = new MyRpcZooKeeper();
    }
    return zooKeeper_;
}

MyRpcZooKeeper::~MyRpcZooKeeper() {
    if(zh_ != nullptr) {
        // 关闭zookeeper句柄,释放资源
        zookeeper_close(zh_);
    }

}


// 全局watcher回调函数,当zookeeper连接成功后，释放信号量,通知主线程,连接成功,继续执行,否则阻塞
// zkserver 给 zkclient 发送一个事件通知,告诉zkclient连接成功
void GlobalWarcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
    if(type == ZOO_SESSION_EVENT) { // 会话事件 
        if(state == ZOO_CONNECTED_STATE) {  // 连接成功
            LOG_INFO("zookeeper connected");
            // 释放信号量
            sem_t *sem = (sem_t *)watcherCtx;   // 传递的信号量
            sem_post(sem);
        }
    }
}


/* 
    zookeeper_mt:多线程版本
    zookeeper的API客户端提供三个线程
    1.API调用线程（当前主线程）
    2.网络I/O线程（zookeeper I/O线程，pthread_create创建,poll监听）
    3.回调线程（watcher线程）
 */
void MyRpcZooKeeper::Start() {
    // 读取配置文件内容
    std::string ip = MyRpcApplication::Instance()->GetConf().Load("zookeeperip");
    std::string port = MyRpcApplication::Instance()->GetConf().Load("zookeeperport");
    std::string host = ip + ":" + port;

    // 创建zookeeper句柄
    zh_ = zookeeper_init(host.c_str(), GlobalWarcher, 30000, nullptr, nullptr, 0);
    if(zh_ == nullptr) {
        LOG_ERROR("zookeeper_init error");
        exit(EXIT_FAILURE);
    }
    // 初始化信号量
    sem_init(&sem_, 0, 0);
    // 设置zookeeper句柄的上下文 
    zoo_set_context(zh_, &sem_);
    // 等待信号量
    sem_wait(&sem_);
    LOG_INFO("zookeeper start success");
}

// 创建节点,如果节点存在则不创建
void MyRpcZooKeeper::CreateNode(const std::string &path, const std::string &data, int datalen, int state)
{
    char pathBuf[128];
    int pathlen = sizeof(pathBuf);
    // 判断节点是否存在
    int flag = zoo_exists(zh_, path.c_str(), 0, nullptr);
    if(flag == ZNONODE) {
        // 创建节点
        flag = zoo_create(zh_, path.c_str(), data.c_str(), datalen, &ZOO_OPEN_ACL_UNSAFE, state, pathBuf, pathlen);
        if(flag == ZOK) {
            LOG_INFO("create node success");
        }
        else {
            LOG_ERROR("zoo_create error:%s, %s", path.c_str(), zerror(flag));
            exit(EXIT_FAILURE);
        }
    }
    else {
        LOG_INFO("node exist");
    }
}

// 获取节点数据
std::string MyRpcZooKeeper::GetNodeData(const std::string &path)
{
    char buf[512];
    int buflen = sizeof(buf);
    int flag = zoo_get(zh_, path.c_str(), 0, buf, &buflen, nullptr);
    if(flag != ZOK) {
        LOG_ERROR("zoo_get data error");
        exit(EXIT_FAILURE);
    }
    return std::string(buf);
}






