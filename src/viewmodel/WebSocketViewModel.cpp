#include "WebSocketViewModel.h"
#include "SensorViewModel.h"
#include <QDebug>

#include "MyToast.h"
#include "model/UserSetting.h"

WebSocketViewModel::WebSocketViewModel(QObject* parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket("", QWebSocketProtocol::VersionLatest, this))
{
    // 连接信号槽
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketViewModel::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketViewModel::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketViewModel::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketViewModel::onBinaryMessageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), 
            this, &WebSocketViewModel::onError);
}

WebSocketViewModel::~WebSocketViewModel() {
    if (m_webSocket) {
        m_webSocket->close();
    }
}

void WebSocketViewModel::connectToServer(const QString& url) {
    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "WebSocket已连接，无需重复连接";
        return;
    }
    
    qDebug() << "正在连接WebSocket服务器:" << url;
    m_webSocket->open(QUrl(url));
}

void WebSocketViewModel::disconnectFromServer() {
    if (m_webSocket) {
        m_webSocket->close();
    }
}

bool WebSocketViewModel::isConnected() const {
    return m_webSocket && m_webSocket->state() == QAbstractSocket::ConnectedState;
}

void WebSocketViewModel::onConnected() {
    qDebug() << "✅ WebSocket连接成功";
    m_state = WAIT_SOF;
    emit connected();
}

void WebSocketViewModel::onDisconnected() {
    qDebug() << "❌ WebSocket已断开";
    m_state = WAIT_SOF;
    m_buffer.clear();
    emit disconnected();
}

void WebSocketViewModel::onTextMessageReceived(const QString& message) {
    // 文本消息转换为二进制处理（直接转发模式）
    QByteArray data = message.toUtf8();
    onBinaryMessageReceived(data);
}

void WebSocketViewModel::onBinaryMessageReceived(const QByteArray& message) {
    qDebug() << "📥 收到WebSocket二进制消息，长度:" << message.size();

    // 处理二进制数据（直接按协议解析）
    for (uint8_t byte : message) {
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
                processFrame(m_cmd, m_buffer);
            } else {
                qDebug() << "❌ CRC校验失败 CMD:" << QString::number(m_cmd, 16);

            }

            m_state = WAIT_SOF;
            break;
        }
        }
    }
}

void WebSocketViewModel::onError(QAbstractSocket::SocketError error) {
    QString errorString;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorString = "连接被拒绝";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorString = "远程主机关闭连接";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorString = "找不到主机";
        break;
    case QAbstractSocket::NetworkError:
        errorString = "网络错误";
        break;
    default:
        errorString = "未知错误";
        break;
    }
    qDebug() << "❌ WebSocket错误:" << errorString;
    emit errorOccurred(errorString);
}


void WebSocketViewModel::processFrame(uint8_t cmd, const QByteArray& data) {
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
        
    case CMD_THRESHOLD:
        if (data.size() == 6) {
            Threshold threshold;
            threshold.fanOffThreshold = data[0];
            threshold.fanOnThreshold = data[1];
            threshold.lampOffThreshold = data[2];
            threshold.lampONThreshold = data[3];
            threshold.DumpOffThreshold = data[4];
            threshold.DumpOnThreshold = data[5];
            emit thresholdReceived(threshold);
        }
        break;
        
    default:
        qDebug() << "⚠️ 未知命令:" << QString::number(cmd, 16);
        break;
    }
}

void WebSocketViewModel::sendFrame(uint8_t cmd, const uint8_t* payload, uint8_t len) {
    if (!isConnected()) {
        qDebug() << "❌ WebSocket未连接，无法发送";
        return;
    }
    
    // 构建完整的协议帧（二进制）
    uint8_t crc = calcCRC(payload, len);
    QByteArray frame;
    frame.append(static_cast<char>(Protocol::SOF));
    frame.append(static_cast<char>(cmd));
    frame.append(static_cast<char>(len));
    frame.append(reinterpret_cast<const char*>(payload), len);
    frame.append(static_cast<char>(crc));
    
    // 以二进制方式发送
    m_webSocket->sendBinaryMessage(frame);
    
    qDebug() << "📤 发送WebSocket二进制帧: CMD=" << QString::number(cmd, 16) << "LEN=" << len;
}

void WebSocketViewModel::sendMotorControl(uint8_t fanStatus, uint8_t fanSpeed, 
                                         uint8_t pumpStatus, uint8_t lampStatus) {
    uint8_t payload[4] = {fanStatus, fanSpeed, pumpStatus, lampStatus};
    sendFrame(CMD_MOTOR_CRTL, payload, 4);
}

void WebSocketViewModel::sendThreshold(uint8_t fanOn, uint8_t fanOff, 
                                      uint8_t pumpOn, uint8_t pumpOff,
                                      uint8_t lampOn, uint8_t lampOff) {
    uint8_t payload[6] = {fanOn, fanOff, pumpOn, pumpOff, lampOn, lampOff};
    sendFrame(CMD_THRESHOLD, payload, 6);
}

void WebSocketViewModel::sendDataCollectControl(bool enable) {
    uint8_t payload[1] = {enable ? (uint8_t)1 : (uint8_t)0};
    sendFrame(CMD_DATA_CRTL, payload, 1);
}

void WebSocketViewModel::sendAutoModeControl(bool enable) {
    uint8_t payload[1] = {enable ? (uint8_t)1 : (uint8_t)0};
    sendFrame(CMD_AUTO_MODE, payload, 1);
}

void WebSocketViewModel::sendGetData(bool enable) {
    uint8_t payload[1] = {enable ? (uint8_t)1 : (uint8_t)0};
    sendFrame(CMD_Get_Date, payload, 1);
}

uint8_t WebSocketViewModel::calcCRC(const uint8_t* data, int len) {
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

