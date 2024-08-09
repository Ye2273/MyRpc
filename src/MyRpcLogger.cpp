#include "MyRpcLogger.h"
#include <time.h>
MyRpcLogger* MyRpcLogger::logger_ = new MyRpcLogger();

MyRpcLogger* MyRpcLogger::GetLogger(){
    return logger_;
}

// 
MyRpcLogger::MyRpcLogger() {
    // 启动一个线程，将日志写入文件
    std::thread t([&]() {
        while (true)
        {
            
            if (!logQue_.Empty())
            {
                std::string log = logQue_.Pop();

                // 获取当前时间，设置日志文件名
                time_t t = time(nullptr);
                tm *local = localtime(&t);
                char logFileName[128];
                
                // snprintf(logFileName, 128, "%d-%02d-%02d.log", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
                // 日志文件名格式：./log/2021-07-01.log
                snprintf(logFileName, 128, "%d-%02d-%02d.log", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
            
                // 写入文件
                FILE *fp = fopen(logFileName, "a+");
                if (fp == nullptr)
                {
                    std::cout << "open file failed" << std::endl;
                    return;
                }
                char tmp[64];
                strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));

                fprintf(fp, "[%s]=>(%s) %s\n", tmp, (logLevel_ == INFO ? "INFO" : "ERROR"), log.c_str());
                fclose(fp);
            }
        }
    });
    // 设置分离线程，守护线程
    t.detach();
}


// 设置日志级别
void MyRpcLogger::SetLogLevel(uint32_t level)
{
    logLevel_ = level;
}

// 将日志写入队列中
void MyRpcLogger::WriteLogQue(const std::string log)
{
    logQue_.Push(log);
}

