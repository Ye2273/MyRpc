#pragma once
#include <iostream>
#include "MyRpcConf.h"
#include <memory>
#include "MyRpcChannel.h"
#include "MyRpcController.h"
/* 
    rpc框架的应用层初始化类
 */
class MyRpcApplication
{
public:
    // 初始化应用层
    void Init(int argc, char **argv);
    static MyRpcApplication* Instance();
    MyRpcConf &GetConf() { return conf_; }
    MyRpcApplication(const MyRpcApplication &) = delete;
    MyRpcApplication &operator=(const MyRpcApplication &) = delete;
private:
    MyRpcApplication(){}
    static MyRpcApplication* app_;
    MyRpcConf conf_;
};