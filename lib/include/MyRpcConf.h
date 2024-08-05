#pragma once

#include <iostream>
#include <unordered_map>


/* 
    rpc框架读取配置文件
    rpc服务提供者的ip和端口号，zookeeper的ip和端口号等
 */
class MyRpcConf
{   
public:
    // 读取配置文件
    void LoadFile(const std::string &file);
    // 获取配置项
    std::string Load(const std::string &key);
    // 去除字符串前后的空格
    std::string Trim(const std::string &str);

private:
    std::unordered_map<std::string, std::string> confMap;
};











