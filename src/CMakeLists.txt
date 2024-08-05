# 查找当前目录下的所有源文件
# aux_source_directory(. SRC_LIST)
set(SRC_LIST MyRpcChannel.cpp MyRpcApplication.cpp 
                        MyRpcProvider.cpp MyRpcConf.cpp 
                        MyRpcMessage.pb.cc MyRpcLogger.cpp
                        MyRpcZooKeeper.cpp) 

# 生成动态库MyRpc
add_library(MyRpc STATIC ${SRC_LIST})
# 链接muduo库
target_link_libraries(MyRpc  muduo_net muduo_base pthread zookeeper_mt)