#pragma once
#include <iostream>
#include <string>
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
class MyRpcZooKeeper
{
public:
    static MyRpcZooKeeper *GetZooKeeper();
    ~MyRpcZooKeeper();
    MyRpcZooKeeper(const MyRpcZooKeeper &) = delete;
    MyRpcZooKeeper &operator=(const MyRpcZooKeeper &) = delete;

    void Start();
    void CreateNode(const std::string &path, const std::string &data, int datalen, int state);
    std::string GetNodeData(const std::string &path);
private:
    MyRpcZooKeeper(){}
    static MyRpcZooKeeper* zooKeeper_;
    sem_t sem_;
    // zookeeper句柄
    zhandle_t *zh_;

};
