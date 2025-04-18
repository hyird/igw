#pragma once
#include <map>
#include <string>
#include <mutex>
#include "variable_reader.h"

// 协议 Reader 注册中心：负责将变量 key 分发到对应 reader
class ReaderRegistry {
public:
    // 注册一个变量 key 到 reader 映射
    void registerReader(const std::string& key, IVariableReader* reader) {
        std::lock_guard<std::mutex> lock(mtx);
        readers[key] = reader;
    }

    // 获取指定 key 对应的 reader（返回 nullptr 表示未注册）
    IVariableReader* getReader(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = readers.find(key);
        return (it != readers.end()) ? it->second : nullptr;
    }

private:
    std::mutex mtx; // 线程安全
    std::map<std::string, IVariableReader*> readers;
};