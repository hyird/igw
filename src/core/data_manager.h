#pragma once
#include "modbus_protocol/modbus_types.h"
#include <mutex>
#include <map>

class DataManager {
public:
    void setValue(const DataPoint& dp);
    DataPoint getValue(const std::string& key);

private:
    std::mutex mtx;
    std::map<std::string, DataPoint> data;
};
