#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <QObject>
#include "../viewmodel/SerialViewModel.h"
#include "../model/ActuatorStateData.h"

/**
 * @brief 设备控制器 - 负责所有执行器的控制逻辑
 * 职责：
 * - 管理设备状态（风扇、灯光、水泵）
 * - 发送控制命令
 * - 同步设备状态
 * - 处理自动/手动模式
 */
class DeviceController : public QObject {
    Q_OBJECT

public:
    explicit DeviceController(QObject* parent = nullptr);
    
    // 设置 SerialViewModel
    void setSerialViewModel(SerialViewModel* viewModel);
    
    // 设备控制
    void toggleFan();
    void toggleLight();
    void togglePump();
    
    // 模式控制
    void setAutoMode(bool enable);
    bool isAutoMode() const { return !m_isLocked; }
    bool isLocked() const { return m_isLocked; }
    
    // 状态查询
    bool isFanOn() const { return m_fanOn; }
    bool isLightOn() const { return m_lightOn; }
    bool isPumpOn() const { return m_pumpOn; }
    
    // 更新设备状态（从下位机接收）
    void updateDeviceState(const ActuatorStateData& data);

signals:
    void fanStateChanged(bool isOn);
    void lightStateChanged(bool isOn);
    void pumpStateChanged(bool isOn);
    void autoModeChanged(bool isAuto);

private:
    void sendControlCommand();
    
    SerialViewModel* m_serialViewModel;
    
    // 设备状态
    bool m_fanOn;
    bool m_lightOn;
    bool m_pumpOn;
    bool m_isLocked;  // true=手动模式, false=自动模式
};

#endif // DEVICECONTROLLER_H

