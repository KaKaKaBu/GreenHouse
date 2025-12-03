#include "SerialViewModel.h"
// viewmodel/SerialViewModel.cpp
#include "SerialViewModel.h"
#include "SensorViewModel.h"
#include "../model/SensorData.h"
#include <QDebug>

SerialViewModel::SerialViewModel(QSerialPort* serialPort, QObject* parent)
    : QObject(parent), m_serial(serialPort) {
    connect(m_serial, &QSerialPort::readyRead, this, &SerialViewModel::onSerialReadyRead);
}

SerialViewModel::~SerialViewModel() = default;

void SerialViewModel::startListening() {
    m_state = WAIT_SOF;
}

void SerialViewModel::stopListening() {
    // 可选：清空缓冲
}

void SerialViewModel::onSerialReadyRead() {
    QByteArray data = m_serial->readAll();
    for (uint8_t byte : data) {
        switch (m_state) {
        case WAIT_SOF:
            if (byte == 0xAA) m_state = WAIT_CMD;
            break;
        case WAIT_CMD:
            m_cmd = byte;
            m_state = WAIT_LEN;
            break;
        case WAIT_LEN:
            m_len = byte;
            m_buffer.resize(m_len);
            m_index = 0;
            m_state = WAIT_DATA;
            break;
        case WAIT_DATA:
            if (m_index < m_len) {
                m_buffer[m_index++] = byte;
                if (m_index >= m_len) m_state = WAIT_CRC;
            }
            break;
        case WAIT_CRC: {
            uint8_t receivedCRC = byte;
            uint8_t calculatedCRC = calcCRC(
                reinterpret_cast<const uint8_t*>(m_buffer.constData()),
                m_len
            );

            if (receivedCRC == calculatedCRC && m_cmd == 0x01 && m_len == 6) {
                // ✅ 有效传感器帧，交给 SensorViewModel 解析
                SensorRecord record = SensorViewModel::parseFromPayload(m_buffer);
                emit sensorDataReceived(record); // 通知上层（如数据库、UI）
            } else if (receivedCRC != calculatedCRC) {
                qDebug() << "❌ CRC 校验失败";
            }

            m_state = WAIT_SOF;
            break;
        }
        }
    }
}

// CRC 算法（与下位机完全一致）
uint8_t SerialViewModel::calcCRC(const uint8_t* data, int len) {
    uint8_t crc = 0;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}
