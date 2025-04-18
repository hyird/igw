#pragma once
#include <sqlite_orm/sqlite_orm.h>
#include "modbus_types.h"

inline auto initStorage(const std::string& dbPath) {
    using namespace sqlite_orm;
    return make_storage(dbPath,
        make_table("modbus_device",
            make_column("id", &ModbusDevice::id, primary_key().autoincrement()),
            make_column("name", &ModbusDevice::name),
            make_column("ip", &ModbusDevice::ip),
            make_column("port", &ModbusDevice::port),
            make_column("slave_id", &ModbusDevice::slave_id)
        ),
        make_table("modbus_variable",
            make_column("id", &ModbusVariable::id, primary_key().autoincrement()),
            make_column("device_id", &ModbusVariable::device_id),
            make_column("variable_key", &ModbusVariable::variable_key),
            make_column("start_addr", &ModbusVariable::start_addr),
            make_column("reg_count", &ModbusVariable::reg_count),
            make_column("poll_interval_ms", &ModbusVariable::poll_interval_ms),
            make_column("data_type", &ModbusVariable::data_type)
        )
    );
}
