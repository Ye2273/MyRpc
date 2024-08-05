#include "MyRpcConf.h"
#include <fstream> 
#include "MyRpcLogger.h"

// 去除字符串前后的空格
std::string MyRpcConf::Trim(const std::string &str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// 读取配置文件
void MyRpcConf::LoadFile(const std::string &file) {
    std::ifstream ifs(file);
    if (!ifs) {
        // std::cout << "config file not exist!" << std::endl;
        LOG_ERROR("config file not exist!");
        exit(0);
    }

    std::string line;
    while (std::getline(ifs, line)) {
        // 跳过空行和注释行
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Trim(line.substr(0, pos));
            std::string value = Trim(line.substr(pos + 1));
            confMap[key] = value;
        }
    }
}

// 获取配置项
std::string MyRpcConf::Load(const std::string &key){
    auto it = confMap.find(key);
    if (it != confMap.end()) {
        return it->second;
    }
    return "";
}

