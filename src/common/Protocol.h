#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>

// 协议命令定义（与Arduino完全一致）
enum CMDType : uint8_t {
    CMD_SENSOR = 0x01,        // 传感器数据
    CMD_MOTOR_STATE = 0x02,   // 电机状态
    CMD_MOTOR_CRTL = 0x03,    // 电机控制
    CMD_THRESHOLD = 0x04,     // 阈值设置
    CMD_HEART_BEAT = 0x05,    // 心跳
    CMD_CRTL_ACK = 0x06,      // 控制应答
    CMD_DATA_CRTL = 0x07,     // 数据采集控制
    CMD_AUTO_MODE = 0x08,     // 自动模式
    CMD_TIME_WEATHER = 0x09,  // 时间天气
    CMD_Get_Date=0x10,
};

// 协议常量
namespace Protocol {
    constexpr uint8_t SOF = 0xAA;  // 帧起始标志
}

#endif // PROTOCOL_H
