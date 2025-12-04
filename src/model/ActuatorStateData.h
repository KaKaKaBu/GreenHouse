#ifndef ACTUATORSTATEDATA_H
#define ACTUATORSTATEDATA_H

#include <cstdint>

/**
 * @brief 电机状态数据（对应下位机 MotorState）
 */
struct ActuatorStateData {
    uint8_t fanStatus = 0;    // 风扇状态 0=关 1=开
    uint8_t fanSpeed = 0;     // 风扇转速 0-100
    uint8_t pumpStatus = 0;   // 水泵状态 0=关 1=开
    uint8_t lampStatus = 0;   // 灯光状态 0=关 1=开
    uint8_t autoMode = 0;     // 自动模式 0=手动 1=自动
};

/**
 * @brief 时间天气数据（对应下位机 TimeWeather）
 */
struct TimeWeatherData {
    uint8_t hour = 0;         // 小时
    uint8_t minute = 0;       // 分钟
    uint8_t weatherCode = 0;  // 天气代码
    int16_t tempNow = 0;      // 当前温度
    int16_t tempLow = 0;      // 最低温度
    int16_t tempHigh = 0;     // 最高温度
};

#endif // ACTUATORSTATEDATA_H

