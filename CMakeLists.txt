cmake_minimum_required(VERSION 3.14)
project(iotgateway)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(libmodbus CONFIG REQUIRED)
find_package(SqliteOrm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.cpp" )

add_executable(iotgateway ${SOURCES})
target_include_directories(iotgateway PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(iotgateway modbus)
target_link_libraries(iotgateway sqlite_orm::sqlite_orm)

