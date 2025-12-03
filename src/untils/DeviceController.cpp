#include "DeviceController.h"
#include <QDebug>

DeviceController::DeviceController(QObject* parent)
    : QObject(parent)
    , m_serialViewModel(nullptr)
    , m_fanOn(false)
    , m_lightOn(false)
    , m_pumpOn(false)
    , m_isLocked(true)  // 默认手动模式
{
}

void DeviceController::setSerialViewModel(SerialViewModel* viewModel) {
    m_serialViewModel = viewModel;
}

void DeviceController::toggleFan() {
    if (!m_isLocked) {
        qDebug() << "DeviceController: 自动模式下无法手动控制风扇";
        return;
    }
    
    m_fanOn = !m_fanOn;
    sendControlCommand();
    emit fanStateChanged(m_fanOn);
    
    qDebug() << "DeviceController: 风扇" << (m_fanOn ? "开启" : "关闭");
}

void DeviceController::toggleLight() {
    if (!m_isLocked) {
        qDebug() << "DeviceController: 自动模式下无法手动控制灯光";
        return;
    }
    
    m_lightOn = !m_lightOn;
    sendControlCommand();
    emit lightStateChanged(m_lightOn);
    
    qDebug() << "DeviceController: 灯光" << (m_lightOn ? "开启" : "关闭");
}

void DeviceController::togglePump() {
    if (!m_isLocked) {
        qDebug() << "DeviceController: 自动模式下无法手动控制水泵";
        return;
    }
    
    m_pumpOn = !m_pumpOn;
    sendControlCommand();
    emit pumpStateChanged(m_pumpOn);
    
    qDebug() << "DeviceController: 水泵" << (m_pumpOn ? "开启" : "关闭");
}

void DeviceController::setAutoMode(bool enable) {
    m_isLocked = !enable;  // locked=true 表示手动模式
    
    if (m_serialViewModel) {
        m_serialViewModel->sendAutoModeControl(enable);
    }
    
    emit autoModeChanged(enable);
    
    qDebug() << "DeviceController: 切换到" << (enable ? "自动" : "手动") << "模式";
}

void DeviceController::updateDeviceState(const ActuatorStateData& data) {
    bool changed = false;
    
    if (m_fanOn != (data.fanStatus == 1)) {
        m_fanOn = (data.fanStatus == 1);
        emit fanStateChanged(m_fanOn);
        changed = true;
    }
    
    if (m_lightOn != (data.lampStatus == 1)) {
        m_lightOn = (data.lampStatus == 1);
        emit lightStateChanged(m_lightOn);
        changed = true;
    }
    
    if (m_pumpOn != (data.pumpStatus == 1)) {
        m_pumpOn = (data.pumpStatus == 1);
        emit pumpStateChanged(m_pumpOn);
        changed = true;
    }
    
    if (changed) {
        qDebug() << "DeviceController: 设备状态已更新 Fan=" << m_fanOn 
                 << "Light=" << m_lightOn 
                 << "Pump=" << m_pumpOn;
    }
}

void DeviceController::sendControlCommand() {
    if (!m_serialViewModel) {
        qDebug() << "DeviceController: SerialViewModel 未设置";
        return;
    }
    
    m_serialViewModel->sendMotorControl(
        m_fanOn ? 1 : 0,
        m_fanOn ? 100 : 0,
        m_pumpOn ? 1 : 0,
        m_lightOn ? 1 : 0
    );
}

