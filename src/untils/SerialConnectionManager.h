#ifndef SERIALCONNECTIONMANAGER_H
#define SERIALCONNECTIONMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QStringList>
#include "../viewmodel/SerialViewModel.h"

/**
 * @brief 串口连接管理器 - 负责串口连接的生命周期管理
 * 职责：
 * - 枚举可用串口
 * - 打开/关闭串口
 * - 管理 SerialViewModel
 * - 提供连接状态
 */
class SerialConnectionManager : public QObject {
    Q_OBJECT

public:
    explicit SerialConnectionManager(QObject* parent = nullptr);
    ~SerialConnectionManager();

    // 串口枚举
    QStringList getAvailablePorts() const;
    
    // 连接管理
    bool connectToPort(const QString& portName);
    void disconnect();
    
    // 状态查询
    bool isConnected() const { return m_connected; }
    QString getCurrentPort() const { return m_currentPort; }
    QString getLastError() const { return m_lastError; }
    
    // 获取 ViewModel
    SerialViewModel* getSerialViewModel() { return m_serialViewModel; }

signals:
    void connected(const QString& portName);
    void disconnected();
    void connectionError(const QString& error);

private:
    QSerialPort* m_serialPort;
    SerialViewModel* m_serialViewModel;
    
    bool m_connected;
    QString m_currentPort;
    QString m_lastError;
    
    // 串口配置常量
    static constexpr int BAUD_RATE = 9600;
};

#endif // SERIALCONNECTIONMANAGER_H

