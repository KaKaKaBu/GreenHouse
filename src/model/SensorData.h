#ifndef SENSORDATA_H
#define SENSORDATA_H

#pragma once
#include <string>

/**
 * @brief 上位机内部使用的传感器数据结构（对应下位机 SensorData）
 */
struct SensorRecord {
    int id = 0;
    std::string record_time;      // "2025-12-03 10:30:00"
    int air_temp = 0;             // ℃
    int air_humid = 0;            // %
    int soil_humid = 0;           // %
    int light_intensity = 0;      // lux 或 raw 值
};
class SensorData
{

};
#endif // SENSORDATA_H
