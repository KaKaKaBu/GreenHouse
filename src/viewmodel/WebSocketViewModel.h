#ifndef WEBSOCKETVIEWMODEL_H
#define WEBSOCKETVIEWMODEL_H

#pragma once
#include <QObject>
#include <QWebSocket>
#include <QByteArray>
#include "../model/SensorData.h"
#include "../model/ActuatorStateData.h"
#include "../common/Protocol.h"
#include "model/UserSetting.h"

class WebSocketViewModel : public QObject {
    Q_OBJECT

public:
    explicit WebSocketViewModel(QObject* parent = nullptr);
    ~WebSocketViewModel();

    void connectToServer(const QString& url);
    void disconnectFromServer();
    bool isConnected() const;
    
    // 发送控制命令
    void sendMotorControl(uint8_t fanStatus, uint8_t fanSpeed, uint8_t pumpStatus, uint8_t lampStatus);
    void sendThreshold(uint8_t fanOn, uint8_t fanOff, uint8_t pumpOn, uint8_t pumpOff, uint8_t lampOn, uint8_t lampOff);
    void sendDataCollectControl(bool enable);
    void sendAutoModeControl(bool enable);
    void sendGetData(bool enable);

signals:
    void sensorDataReceived(const SensorRecord& data);         // 传感器数据
    void actuatorStateReceived(const ActuatorStateData& data); // 电机状态
    void timeWeatherReceived(const TimeWeatherData& data);     // 时间天气
    void heartBeatReceived();                                  // 心跳包接收
    void thresholdReceived(const Threshold &threshold);
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onBinaryMessageReceived(const QByteArray& message);
    void onError(QAbstractSocket::SocketError error);

private:
    enum State {
        WAIT_SOF,
        WAIT_CMD,
        WAIT_LEN,
        WAIT_DATA,
        WAIT_CRC
    };

    QWebSocket* m_webSocket;
    State m_state = WAIT_SOF;
    uint8_t m_cmd = 0;
    uint8_t m_len = 0;
    QByteArray m_buffer;
    int m_index = 0;

    uint8_t calcCRC(const uint8_t* data, int len);  // CRC-8计算
    void processFrame(uint8_t cmd, const QByteArray& data);  // 处理接收到的帧
    void sendFrame(uint8_t cmd, const uint8_t* payload, uint8_t len);  // 发送帧
};

#endif // WEBSOCKETVIEWMODEL_H

