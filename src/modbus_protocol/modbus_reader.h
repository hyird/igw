#pragma once
#include "modbus_protocol/modbus_types.h"
#include "core/data_manager.h"
#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>

class ModbusReader {
public:
    ModbusReader(const ModbusDevice& dev, const std::vector<ModbusVariable>& vars, std::shared_ptr<DataManager> dm);
    void start();
    void stop();
    void scheduleWrite(const ModbusVariable& var, uint16_t value); // 添加写入调度接口
private:
    void run();

    enum class TaskType { Read, Write };
    struct Task {
        std::chrono::steady_clock::time_point next_time;
        ModbusVariable var;
        TaskType type;
        uint16_t write_value = 0; // 仅用于写任务
        bool operator>(const Task& other) const {
            return next_time > other.next_time;
        }
    };

    ModbusDevice device;
    std::vector<ModbusVariable> variables;
    std::shared_ptr<DataManager> dataManager;
    std::priority_queue<Task, std::vector<Task>, std::greater<>> taskQueue;
    std::mutex queueMutex;

    std::thread worker;
    std::atomic<bool> running{false};
};
