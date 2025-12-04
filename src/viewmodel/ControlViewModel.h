#ifndef CONTROLVIEWMODEL_H
#define CONTROLVIEWMODEL_H

#pragma once
#include <QObject>
#include "../model/ActuatorStateData.h"

/**
 * @brief 设备控制 ViewModel
 * 
 * 职责：
 * - 管理设备控制命令的构建
 * - 维护设备当前状态
 * - 提供设备控制的业务逻辑
 * - 验证控制命令的合法性
 */
class ControlViewModel : public QObject {
    Q_OBJECT

public:
    explicit ControlViewModel(QObject* parent = nullptr);
    ~ControlViewModel();

    // ========== 状态查询 ==========
    bool isFanOn() const { return m_currentState.fanStatus == 1; }
    bool isPumpOn() const { return m_currentState.pumpStatus == 1; }
    bool isLampOn() const { return m_currentState.lampStatus == 1; }
    bool isAutoMode() const { return m_currentState.autoMode == 1; }
    uint8_t getFanSpeed() const { return m_currentState.fanSpeed; }
    
    const ActuatorStateData& getCurrentState() const { return m_currentState; }

    // ========== 命令构建 ==========
    
    /**
     * @brief 构建电机控制命令的 payload
     * @param fanStatus 风扇状态（0=关, 1=开）
     * @param fanSpeed 风扇速度（0-100）
     * @param pumpStatus 水泵状态（0=关, 1=开）
     * @param lampStatus 灯光状态（0=关, 1=开）
     * @return payload 字节数组（4字节）
     */
    static QByteArray buildMotorControlPayload(uint8_t fanStatus, uint8_t fanSpeed, 
                                                uint8_t pumpStatus, uint8_t lampStatus);
    
    /**
     * @brief 构建阈值设置命令的 payload
     * @param fanOn 风扇开启温度阈值
     * @param fanOff 风扇关闭温度阈值
     * @param pumpOn 水泵开启土壤湿度阈值
     * @param pumpOff 水泵关闭土壤湿度阈值
     * @param lampOn 灯光开启光照阈值
     * @param lampOff 灯光关闭光照阈值
     * @return payload 字节数组（6字节）
     */
    static QByteArray buildThresholdPayload(uint8_t fanOn, uint8_t fanOff,
                                             uint8_t pumpOn, uint8_t pumpOff,
                                             uint8_t lampOn, uint8_t lampOff);
    
    /**
     * @brief 构建数据采集控制命令的 payload
     * @param enable true=开始采集, false=停止采集
     * @return payload 字节数组（1字节）
     */
    static QByteArray buildDataCollectPayload(bool enable);
    
    /**
     * @brief 构建自动模式控制命令的 payload
     * @param enable true=自动模式, false=手动模式
     * @return payload 字节数组（1字节）
     */
    static QByteArray buildAutoModePayload(bool enable);

    // ========== 状态更新 ==========
    
    /**
     * @brief 更新设备当前状态
     * @param state 新的设备状态
     */
    void updateState(const ActuatorStateData& state);
    
    /**
     * @brief 切换风扇状态
     * @return 新的风扇状态（true=开, false=关）
     */
    bool toggleFan();
    
    /**
     * @brief 切换水泵状态
     * @return 新的水泵状态（true=开, false=关）
     */
    bool togglePump();
    
    /**
     * @brief 切换灯光状态
     * @return 新的灯光状态（true=开, false=关）
     */
    bool toggleLamp();
    
    /**
     * @brief 切换自动/手动模式
     * @return 新的模式（true=自动, false=手动）
     */
    bool toggleAutoMode();
    
    /**
     * @brief 设置风扇速度
     * @param speed 速度值（0-100）
     * @return true=设置成功, false=速度值非法
     */
    bool setFanSpeed(uint8_t speed);

    // ========== 命令验证 ==========
    
    /**
     * @brief 验证风扇速度是否合法
     * @param speed 速度值
     * @return true=合法（0-100）, false=非法
     */
    static bool isValidFanSpeed(uint8_t speed);
    
    /**
     * @brief 验证阈值是否合法
     * @param onValue 开启阈值
     * @param offValue 关闭阈值
     * @return true=合法（onValue > offValue）, false=非法
     */
    static bool isValidThreshold(uint8_t onValue, uint8_t offValue);

signals:
    /**
     * @brief 设备状态发生变化
     * @param state 新的设备状态
     */
    void stateChanged(const ActuatorStateData& state);
    
    /**
     * @brief 风扇状态变化
     * @param isOn true=开启, false=关闭
     */
    void fanStateChanged(bool isOn);
    
    /**
     * @brief 水泵状态变化
     * @param isOn true=开启, false=关闭
     */
    void pumpStateChanged(bool isOn);
    
    /**
     * @brief 灯光状态变化
     * @param isOn true=开启, false=关闭
     */
    void lampStateChanged(bool isOn);
    
    /**
     * @brief 自动模式状态变化
     * @param isAuto true=自动模式, false=手动模式
     */
    void autoModeChanged(bool isAuto);
    
    /**
     * @brief 风扇速度变化
     * @param speed 新的速度值（0-100）
     */
    void fanSpeedChanged(uint8_t speed);

private:
    ActuatorStateData m_currentState;  // 当前设备状态
};

#endif // CONTROLVIEWMODEL_H
