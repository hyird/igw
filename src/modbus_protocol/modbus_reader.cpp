#include "modbus_reader.h"
#include <modbus.h>
#include <chrono>
#include <iostream>
#include <ctime>


ModbusReader::ModbusReader(const ModbusDevice& dev, const std::vector<ModbusVariable>& vars, std::shared_ptr<DataManager> dm)
    : device(dev), variables(vars), dataManager(dm) {}

void ModbusReader::start() {
    running = true;
    worker = std::thread([this]() { this->run(); });
}

void ModbusReader::stop() {
    running = false;
    if (worker.joinable()) worker.join();
}

void ModbusReader::scheduleWrite(const ModbusVariable& var, uint16_t value) {
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(Task{
        std::chrono::steady_clock::now(),
        var,
        TaskType::Write,
        value
    });
}

void ModbusReader::run() {
    modbus_t* ctx = modbus_new_tcp(device.ip.c_str(), device.port);
    if (modbus_connect(ctx) == -1) {
        std::cerr << "Modbus connect failed: " << modbus_strerror(errno) << "\n";
        modbus_free(ctx);
        return;
    }
    modbus_set_slave(ctx, device.slave_id);

    auto now = std::chrono::steady_clock::now();
    for (const auto& var : variables) {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(Task{now + std::chrono::milliseconds(var.poll_interval_ms), var, TaskType::Read});
    }

    while (running) {
        Task task;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (taskQueue.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            task = taskQueue.top();
            if (task.next_time > std::chrono::steady_clock::now()) {
                std::this_thread::sleep_until(task.next_time);
                continue;
            }
            taskQueue.pop();
        }

        if (task.type == TaskType::Read) {
            std::vector<uint16_t> buffer(task.var.reg_count);
            int rc = modbus_read_registers(ctx, task.var.start_addr, task.var.reg_count, buffer.data());

            auto sysNow = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(sysNow);
            char buf[64];
            strftime(buf, sizeof(buf), "%F %T", std::localtime(&t));
            std::string ts = buf;

            DataPoint dp;
            dp.key = task.var.variable_key;
            dp.timestamp = ts;
            dp.type = task.var.data_type;
            dp.value = (rc > 0) ? std::to_string(buffer[0]) : "0";
            dp.quality = (rc > 0) ? "GOOD" : "BAD";
            dataManager->setValue(dp);

            std::lock_guard<std::mutex> lock(queueMutex);
            task.next_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(task.var.poll_interval_ms);
            taskQueue.push(task);
        }
        else if (task.type == TaskType::Write) {
            int rc = modbus_write_register(ctx, task.var.start_addr, task.write_value);
            std::cout << "[WRITE] " << task.var.variable_key << " <- " << task.write_value
                << (rc > 0 ? " [OK]" : " [FAIL]") << std::endl;
        }
    }

    modbus_close(ctx);
    modbus_free(ctx);
}
