#include "data_manager.h"

void DataManager::setValue(const DataPoint& dp) {
    std::lock_guard<std::mutex> lock(mtx);
    data[dp.key] = dp;
}

DataPoint DataManager::getValue(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    return data[key];
}