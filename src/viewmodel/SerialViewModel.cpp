#include "SerialViewModel.h"
#include "SensorViewModel.h"
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
    m_state = WAIT_SOF;
    m_buffer.clear();
}

void SerialViewModel::onSerialReadyRead() {
    QByteArray data = m_serial->readAll();
    for (uint8_t byte : data) {
        switch (m_state) {
        case WAIT_SOF:
            if (byte == Protocol::SOF) {
                m_state = WAIT_CMD;
            }
            break;
        case WAIT_CMD:
            m_cmd = byte;
            m_state = WAIT_LEN;
            break;
        case WAIT_LEN:
            m_len = byte;
            m_buffer.resize(m_len);
            m_index = 0;
            m_state = (m_len > 0) ? WAIT_DATA : WAIT_CRC;
            break;
        case WAIT_DATA:
            if (m_index < m_len) {
                m_buffer[m_index++] = byte;
                if (m_index >= m_len) {
                    m_state = WAIT_CRC;
                }
            }
            break;
        case WAIT_CRC: {
            uint8_t receivedCRC = byte;
            uint8_t calculatedCRC = calcCRC(
                reinterpret_cast<const uint8_t*>(m_buffer.constData()),
                m_len
            );

            if (receivedCRC == calculatedCRC) {
                // CRC校验成功，处理帧
                processFrame(m_cmd, m_buffer);
            } else {
                qDebug() << "❌ CRC校验失败 CMD:" << QString::number(m_cmd, 16) 
                         << "Expected:" << QString::number(calculatedCRC, 16)
                         << "Received:" << QString::number(receivedCRC, 16);
            }

            m_state = WAIT_SOF;
            break;
        }
        }
    }
}

// 处理接收到的完整帧
void SerialViewModel::processFrame(uint8_t cmd, const QByteArray& data) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data.constData());
    
    switch (cmd) {
    case CMD_SENSOR:  // 传感器数据
        if (data.size() == 6) {
            SensorRecord record = SensorViewModel::parseFromPayload(data);
            emit sensorDataReceived(record);
            qDebug() << "✅ 接收传感器数据: Temp=" << record.air_temp 
                     << "AirHum=" << record.air_humid 
                     << "SoilHum=" << record.soil_humid
                     << "Light=" << record.light_intensity;
        }
        break;
        
    case CMD_MOTOR_STATE:  // 电机状态
        if (data.size() == 5) {
            ActuatorStateData state;
            state.fanStatus = p[0];
            state.fanSpeed = p[1];
            state.pumpStatus = p[2];
            state.lampStatus = p[3];
            state.autoMode = p[4];
            emit actuatorStateReceived(state);
            qDebug() << "✅ 接收电机状态: Fan=" << state.fanStatus 
                     << "Pump=" << state.pumpStatus 
                     << "Lamp=" << state.lampStatus
                     << "Auto=" << state.autoMode;
        }
        break;
        
    case CMD_TIME_WEATHER:  // 时间天气
        if (data.size() == 8) {
            TimeWeatherData weather;
            weather.hour = p[0];
            weather.minute = p[1];
            weather.weatherCode = p[2];
            weather.tempNow = (static_cast<int16_t>(p[3]) << 8) | p[4];
            weather.tempLow = p[5];
            weather.tempHigh = p[6];
            emit timeWeatherReceived(weather);
            qDebug() << "✅ 接收时间天气:" << weather.hour << ":" << weather.minute
                     << "天气码=" << weather.weatherCode << "温度=" << weather.tempNow;
        }
        break;
        
    case CMD_CRTL_ACK:  // 控制应答
        if (data.size() == 2) {
            uint8_t originalCmd = p[0];
            uint8_t result = p[1];
            const char* cmdName = "UNKNOWN";
            
            // 根据命令类型显示不同的名称
            switch (originalCmd) {
                case CMD_MOTOR_CRTL: cmdName = "电机控制"; break;
                case CMD_THRESHOLD: cmdName = "阈值设置"; break;
                case CMD_DATA_CRTL: cmdName = "数据采集"; break;
                case CMD_AUTO_MODE: cmdName = "模式切换"; break;
                case CMD_TIME_WEATHER: cmdName = "时间天气"; break;
            }
            
            qDebug() << "✅ 接收控制应答:" << cmdName 
                     << "CMD=" << QString::number(originalCmd, 16) 
                     << "Result=" << (result == 0x01 ? "成功" : "失败");
        }
        break;
    
    case CMD_HEART_BEAT:  // 心跳包
        if (data.size() == 1) {
            uint8_t status = p[0];
            qDebug() << "💓 接收心跳包: 设备状态=" << (status == 0x01 ? "正常" : "异常");
            emit heartBeatReceived();
        }
        break;
        
    default:
        qDebug() << "⚠️ 未知命令:" << QString::number(cmd, 16);
        break;
    }
}

// 发送帧（通用）
void SerialViewModel::sendFrame(uint8_t cmd, const uint8_t* payload, uint8_t len) {
    if (!m_serial || !m_serial->isOpen()) {
        qDebug() << "❌ 串口未打开，无法发送";
        return;
    }
    
    uint8_t crc = calcCRC(payload, len);
    
    m_serial->write(reinterpret_cast<const char*>(&Protocol::SOF), 1);
    m_serial->write(reinterpret_cast<const char*>(&cmd), 1);
    m_serial->write(reinterpret_cast<const char*>(&len), 1);
    m_serial->write(reinterpret_cast<const char*>(payload), len);
    m_serial->write(reinterpret_cast<const char*>(&crc), 1);
    
    qDebug() << "📤 发送帧: CMD=" << QString::number(cmd, 16) << "LEN=" << len;
}

// 发送电机控制命令
void SerialViewModel::sendMotorControl(uint8_t fanStatus, uint8_t fanSpeed, 
                                       uint8_t pumpStatus, uint8_t lampStatus) {
    uint8_t payload[4] = {fanStatus, fanSpeed, pumpStatus, lampStatus};
    sendFrame(CMD_MOTOR_CRTL, payload, 4);
    qDebug() << "📤 发送电机控制: Fan=" << fanStatus << "/" << fanSpeed 
             << "Pump=" << pumpStatus << "Lamp=" << lampStatus;
}

// 发送阈值设置
void SerialViewModel::sendThreshold(uint8_t fanOn, uint8_t fanOff, 
                                    uint8_t pumpOn, uint8_t pumpOff,
                                    uint8_t lampOn, uint8_t lampOff) {
    uint8_t payload[6] = {fanOn, fanOff, pumpOn, pumpOff, lampOn, lampOff};
    sendFrame(CMD_THRESHOLD, payload, 6);
    qDebug() << "📤 发送阈值设置";
}

// 发送数据采集控制
void SerialViewModel::sendDataCollectControl(bool enable) {
    uint8_t payload[1] = {enable ? (uint8_t)1 : (uint8_t)0};
    sendFrame(CMD_DATA_CRTL, payload, 1);
    qDebug() << "📤 发送数据采集控制:" << (enable ? "启用" : "禁用");
}

// 发送自动模式控制
void SerialViewModel::sendAutoModeControl(bool enable) {
    uint8_t payload[1] = {enable ? (uint8_t)1 : (uint8_t)0};
    sendFrame(CMD_AUTO_MODE, payload, 1);
    qDebug() << "📤 发送自动模式控制:" << (enable ? "启用" : "禁用");
}

// CRC-8算法（与下位机完全一致）
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
