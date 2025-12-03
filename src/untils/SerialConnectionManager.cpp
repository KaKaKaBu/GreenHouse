#include "SerialConnectionManager.h"
#include <QDebug>

SerialConnectionManager::SerialConnectionManager(QObject* parent)
    : QObject(parent)
    , m_serialPort(nullptr)
    , m_serialViewModel(nullptr)
    , m_connected(false)
{
    m_serialPort = new QSerialPort(this);
}

SerialConnectionManager::~SerialConnectionManager() {
    disconnect();
}

QStringList SerialConnectionManager::getAvailablePorts() const {
    QStringList ports;
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    
    for (const QSerialPortInfo& info : availablePorts) {
        ports.append(info.portName());
    }
    
    qDebug() << "SerialConnectionManager: 可用串口" << ports;
    return ports;
}

bool SerialConnectionManager::connectToPort(const QString& portName) {
    if (m_connected) {
        m_lastError = "串口已连接";
        return false;
    }
    
    if (portName.isEmpty()) {
        m_lastError = "串口名称为空";
        return false;
    }
    
    // 配置串口
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(BAUD_RATE);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    // 打开串口
    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        m_lastError = m_serialPort->errorString();
        qDebug() << "SerialConnectionManager: 连接失败" << m_lastError;
        return false;
    }
    
    // 创建 SerialViewModel
    m_serialViewModel = new SerialViewModel(m_serialPort, this);
    m_serialViewModel->startListening();
    
    // 启用数据采集
    m_serialViewModel->sendDataCollectControl(true);
    
    m_connected = true;
    m_currentPort = portName;
    m_lastError.clear();
    
    qDebug() << "SerialConnectionManager: 连接成功" << portName;
    emit connected(portName);
    
    return true;
}

void SerialConnectionManager::disconnect() {
    if (!m_connected) {
        return;
    }
    
    // 停止监听
    if (m_serialViewModel) {
        m_serialViewModel->stopListening();
        delete m_serialViewModel;
        m_serialViewModel = nullptr;
    }
    
    // 关闭串口
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    
    m_connected = false;
    m_currentPort.clear();
    
    qDebug() << "SerialConnectionManager: 已断开";
    emit disconnected();
}

