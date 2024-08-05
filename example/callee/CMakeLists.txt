set(SRC_LIST UserService.cpp ../UserProto.pb.cc)
# 生成可执行文件Provider
add_executable(Provider ${SRC_LIST})
# Provider 链接库文件：MyRpc和protobuf
target_link_libraries(Provider MyRpc protobuf)


