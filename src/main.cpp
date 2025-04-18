#include "modbus_protocol/modbus_storage.h"
#include "modbus_protocol/modbus_reader.h"
#include "core/data_manager.h"

#include <memory>
#include <map>
#include <vector>
#include <thread>

int main() {
    // 初始化数据库
    auto storage = initStorage("config.db");
    storage.sync_schema(); // 创建表结构（若不存在）

    // 加载设备与变量配置
    auto devices = storage.get_all<ModbusDevice>();
    auto variables = storage.get_all<ModbusVariable>();
    auto dataManager = std::make_shared<DataManager>();

    // 按设备 ID 分组变量
    std::map<int, std::vector<ModbusVariable>> grouped;
    for (const auto& var : variables) {
        grouped[var.device_id].push_back(var);
    }

    // 为每个设备创建一个 ModbusReader（独立线程）
    std::vector<std::unique_ptr<ModbusReader>> readers;
    for (const auto& device : devices) {
        if (grouped.count(device.id)) {
            auto reader = std::make_unique<ModbusReader>(device, grouped[device.id], dataManager);
            reader->start();
            readers.push_back(std::move(reader));
        }
    }

    // 简单运行一段时间（测试用）
    std::this_thread::sleep_for(std::chrono::seconds(999));

    // 停止所有设备采集线程
    for (auto& reader : readers) {
        reader->stop();
    }

    return 0;
}
