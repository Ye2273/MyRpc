#pragma once
#include <iostream>
#include <string>
#include "MyRpcLogger.h"
#include "MyRpcLogQue.h"
enum LogLevel
{
    INFO,
    ERROR,
};

// 日志类
class MyRpcLogger
{
public:
    static MyRpcLogger *GetLogger();
    MyRpcLogger(const MyRpcLogger &) = delete;
    MyRpcLogger &operator=(const MyRpcLogger &) = delete;
    void SetLogLevel(uint32_t level);
    void WriteLogQue(const std::string log);
private:
    MyRpcLogger();
    static MyRpcLogger *logger_;
    uint32_t logLevel_;
    MyRpcLogQue<std::string> logQue_;
};

// 日志宏定义:形式为LOG_INFO("log message %d", 1);
#define LOG_INFO(logmsgformat, ...) \
        do { \
            MyRpcLogger* logger = MyRpcLogger::GetLogger(); \
            logger->SetLogLevel(INFO); \
            char logmsg[1024]; \
            snprintf(logmsg, 1024, logmsgformat, ##__VA_ARGS__); \
            logger->WriteLogQue(logmsg); \
        } while(0)

#define LOG_ERROR(logmsgformat, ...) \
        do { \
            MyRpcLogger* logger = MyRpcLogger::GetLogger(); \
            logger->SetLogLevel(ERROR); \
            char logmsg[1024]; \
            snprintf(logmsg, 1024, logmsgformat, ##__VA_ARGS__); \
            logger->WriteLogQue(logmsg); \
        } while(0)
