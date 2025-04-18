#pragma once
#include "modbus/modbus_types.h"

// 通用接口：所有协议采集器都应实现这个类
class IVariableReader {
public:
    // 调度一个读任务
    virtual void scheduleRead(const ModbusVariable& var) = 0;

    // 调度一个写任务（写一个值到指定变量）
    virtual void scheduleWrite(const ModbusVariable& var, uint16_t value) = 0;

    // 虚析构函数以支持多态
    virtual ~IVariableReader() = default;
};