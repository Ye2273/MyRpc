set(SRC_LIST UserConsumer.cpp ../UserProto.pb.cc)
# 生成可执行文件Provider
add_executable(Consumer ${SRC_LIST})
# Provider 链接库文件：MyRpc和protobuf
target_link_libraries(Consumer MyRpc protobuf)


