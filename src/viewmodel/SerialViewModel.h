#ifndef SERIALVIEWMODEL_H
#define SERIALVIEWMODEL_H

#pragma once
#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include "../model/SensorData.h"
#include "../model/ActuatorStateData.h"
#include "../common/Protocol.h"

class SerialViewModel : public QObject {
    Q_OBJECT

public:
    explicit SerialViewModel(QSerialPort* serialPort, QObject* parent = nullptr);
    ~SerialViewModel();

    void startListening();
    void stopListening();
    
    // 发送控制命令
    void sendMotorControl(uint8_t fanStatus, uint8_t fanSpeed, uint8_t pumpStatus, uint8_t lampStatus);
    void sendThreshold(uint8_t fanOn, uint8_t fanOff, uint8_t pumpOn, uint8_t pumpOff, uint8_t lampOn, uint8_t lampOff);
    void sendDataCollectControl(bool enable);
    void sendAutoModeControl(bool enable);

signals:
    void sensorDataReceived(const SensorRecord& data);         // 传感器数据
    void actuatorStateReceived(const ActuatorStateData& data); // 电机状态
    void timeWeatherReceived(const TimeWeatherData& data);     // 时间天气

private slots:
    void onSerialReadyRead();

private:
    enum State {
        WAIT_SOF,
        WAIT_CMD,
        WAIT_LEN,
        WAIT_DATA,
        WAIT_CRC
    };

    QSerialPort* m_serial;
    State m_state = WAIT_SOF;
    uint8_t m_cmd = 0;
    uint8_t m_len = 0;
    QByteArray m_buffer;
    int m_index = 0;

    uint8_t calcCRC(const uint8_t* data, int len);  // CRC-8计算
    void processFrame(uint8_t cmd, const QByteArray& data);  // 处理接收到的帧
    void sendFrame(uint8_t cmd, const uint8_t* payload, uint8_t len);  // 发送帧
};

#endif // SERIALVIEWMODEL_H
