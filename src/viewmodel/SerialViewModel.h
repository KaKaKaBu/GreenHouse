#ifndef SERIALVIEWMODEL_H
#define SERIALVIEWMODEL_H

// viewmodel/SerialViewModel.h
#pragma once
#include <QObject>
#include <QSerialPort>
#include <QByteArray>

class SensorViewModel; // 前向声明

class SerialViewModel : public QObject {
    Q_OBJECT

public:
    explicit SerialViewModel(QSerialPort* serialPort, QObject* parent = nullptr);
    ~SerialViewModel();

    void startListening();
    void stopListening();

    signals:
        void sensorDataReceived(const struct SensorRecord& data); // 转发给数据库/UI

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

    uint8_t calcCRC(const uint8_t* data, int len); // 与下位机一致
};

#endif // SERIALVIEWMODEL_H
