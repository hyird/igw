#pragma once
#include <string>

struct ModbusDevice {
    int id;
    std::string name;
    std::string ip;
    int port;
    int slave_id;
};

struct ModbusVariable {
    int id;
    int device_id;
    std::string variable_key;
    int start_addr;
    int reg_count;
    int poll_interval_ms;
    std::string data_type;
};

struct DataPoint {
    std::string key;
    std::string value;
    std::string quality;
    std::string timestamp;
    std::string type;
};
