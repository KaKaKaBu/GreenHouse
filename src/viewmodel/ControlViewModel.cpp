#include "ControlViewModel.h"
#include <QDebug>

ControlViewModel::ControlViewModel(QObject* parent)
    : QObject(parent) {
    // 初始化为默认状态（全部关闭，手动模式）
    m_currentState.fanStatus = 0;
    m_currentState.fanSpeed = 0;
    m_currentState.pumpStatus = 0;
    m_currentState.lampStatus = 0;
    m_currentState.autoMode = 0;
    
    qDebug() << "🎮 ControlViewModel 初始化完成";
}

ControlViewModel::~ControlViewModel() = default;

// ========================================
// 命令构建
// ========================================

QByteArray ControlViewModel::buildMotorControlPayload(uint8_t fanStatus, uint8_t fanSpeed,
                                                       uint8_t pumpStatus, uint8_t lampStatus) {
    // 验证参数
    if (fanStatus > 1 || pumpStatus > 1 || lampStatus > 1) {
        qWarning() << "⚠️ 无效的状态值：status 必须为 0 或 1";
        return QByteArray();
    }
    
    if (!isValidFanSpeed(fanSpeed)) {
        qWarning() << "⚠️ 无效的风扇速度：" << fanSpeed;
        return QByteArray();
    }
    
    QByteArray payload;
    payload.append(static_cast<char>(fanStatus));
    payload.append(static_cast<char>(fanSpeed));
    payload.append(static_cast<char>(pumpStatus));
    payload.append(static_cast<char>(lampStatus));
    
    qDebug() << "📦 构建电机控制 payload:"
             << "Fan=" << fanStatus << "Speed=" << fanSpeed
             << "Pump=" << pumpStatus << "Lamp=" << lampStatus;
    
    return payload;
}

QByteArray ControlViewModel::buildThresholdPayload(uint8_t fanOn, uint8_t fanOff,
                                                     uint8_t pumpOn, uint8_t pumpOff,
                                                     uint8_t lampOn, uint8_t lampOff) {
    // 验证阈值合法性
    if (!isValidThreshold(fanOn, fanOff)) {
        qWarning() << "⚠️ 无效的风扇阈值：ON=" << fanOn << "OFF=" << fanOff;
        return QByteArray();
    }
    
    if (!isValidThreshold(pumpOn, pumpOff)) {
        qWarning() << "⚠️ 无效的水泵阈值：ON=" << pumpOn << "OFF=" << pumpOff;
        return QByteArray();
    }
    
    if (!isValidThreshold(lampOn, lampOff)) {
        qWarning() << "⚠️ 无效的灯光阈值：ON=" << lampOn << "OFF=" << lampOff;
        return QByteArray();
    }
    
    QByteArray payload;
    payload.append(static_cast<char>(fanOn));
    payload.append(static_cast<char>(fanOff));
    payload.append(static_cast<char>(pumpOn));
    payload.append(static_cast<char>(pumpOff));
    payload.append(static_cast<char>(lampOn));
    payload.append(static_cast<char>(lampOff));
    
    qDebug() << "📦 构建阈值设置 payload:"
             << "Fan[" << fanOn << "-" << fanOff << "]"
             << "Pump[" << pumpOn << "-" << pumpOff << "]"
             << "Lamp[" << lampOn << "-" << lampOff << "]";
    
    return payload;
}

QByteArray ControlViewModel::buildDataCollectPayload(bool enable) {
    QByteArray payload;
    payload.append(static_cast<char>(enable ? 1 : 0));
    
    qDebug() << "📦 构建数据采集控制 payload:" << (enable ? "开启" : "停止");
    
    return payload;
}

QByteArray ControlViewModel::buildAutoModePayload(bool enable) {
    QByteArray payload;
    payload.append(static_cast<char>(enable ? 1 : 0));
    
    qDebug() << "📦 构建自动模式控制 payload:" << (enable ? "自动" : "手动");
    
    return payload;
}

// ========================================
// 状态更新
// ========================================

void ControlViewModel::updateState(const ActuatorStateData& state) {
    bool fanChanged = (m_currentState.fanStatus != state.fanStatus);
    bool pumpChanged = (m_currentState.pumpStatus != state.pumpStatus);
    bool lampChanged = (m_currentState.lampStatus != state.lampStatus);
    bool autoModeChangedFlag = (m_currentState.autoMode != state.autoMode);
    bool fanSpeedChangedFlag = (m_currentState.fanSpeed != state.fanSpeed);
    
    m_currentState = state;
    
    // 发射状态变化信号
    emit stateChanged(m_currentState);
    
    if (fanChanged) {
        emit fanStateChanged(isFanOn());
        qDebug() << "🌀 风扇状态变化:" << (isFanOn() ? "开启" : "关闭");
    }
    
    if (pumpChanged) {
        emit pumpStateChanged(isPumpOn());
        qDebug() << "💧 水泵状态变化:" << (isPumpOn() ? "开启" : "关闭");
    }
    
    if (lampChanged) {
        emit lampStateChanged(isLampOn());
        qDebug() << "💡 灯光状态变化:" << (isLampOn() ? "开启" : "关闭");
    }
    
    if (autoModeChangedFlag) {
        emit autoModeChanged(isAutoMode());
        qDebug() << "🤖 模式变化:" << (isAutoMode() ? "自动" : "手动");
    }
    
    if (fanSpeedChangedFlag) {
        emit fanSpeedChanged(state.fanSpeed);
        qDebug() << "⚡ 风扇速度变化:" << state.fanSpeed;
    }
}

bool ControlViewModel::toggleFan() {
    m_currentState.fanStatus = m_currentState.fanStatus ? 0 : 1;
    emit fanStateChanged(isFanOn());
    emit stateChanged(m_currentState);
    
    qDebug() << "🌀 切换风扇:" << (isFanOn() ? "开启" : "关闭");
    
    return isFanOn();
}

bool ControlViewModel::togglePump() {
    m_currentState.pumpStatus = m_currentState.pumpStatus ? 0 : 1;
    emit pumpStateChanged(isPumpOn());
    emit stateChanged(m_currentState);
    
    qDebug() << "💧 切换水泵:" << (isPumpOn() ? "开启" : "关闭");
    
    return isPumpOn();
}

bool ControlViewModel::toggleLamp() {
    m_currentState.lampStatus = m_currentState.lampStatus ? 0 : 1;
    emit lampStateChanged(isLampOn());
    emit stateChanged(m_currentState);
    
    qDebug() << "💡 切换灯光:" << (isLampOn() ? "开启" : "关闭");
    
    return isLampOn();
}

bool ControlViewModel::toggleAutoMode() {
    m_currentState.autoMode = m_currentState.autoMode ? 0 : 1;
    emit autoModeChanged(isAutoMode());
    emit stateChanged(m_currentState);
    
    qDebug() << "🤖 切换模式:" << (isAutoMode() ? "自动" : "手动");
    
    return isAutoMode();
}

bool ControlViewModel::setFanSpeed(uint8_t speed) {
    if (!isValidFanSpeed(speed)) {
        qWarning() << "⚠️ 无效的风扇速度：" << speed;
        return false;
    }
    
    m_currentState.fanSpeed = speed;
    emit fanSpeedChanged(speed);
    emit stateChanged(m_currentState);
    
    qDebug() << "⚡ 设置风扇速度:" << speed;
    
    return true;
}

// ========================================
// 命令验证
// ========================================

bool ControlViewModel::isValidFanSpeed(uint8_t speed) {
    return speed <= 100;
}

bool ControlViewModel::isValidThreshold(uint8_t onValue, uint8_t offValue) {
    // 开启阈值必须大于关闭阈值，确保有滞后效应
    return onValue > offValue;
}
