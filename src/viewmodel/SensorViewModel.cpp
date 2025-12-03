#include "SensorViewModel.h"
// viewmodel/SensorViewModel.cpp
#include "SensorViewModel.h"
#include <QDateTime>
#include <QDebug>

SensorViewModel::SensorViewModel(QObject* parent)
    : QObject(parent) {}

// 辅助函数：大端转 int16_t
static int16_t bigEndianToInt16(uint8_t high, uint8_t low) {
    return (static_cast<int16_t>(high) << 8) | low;
}

SensorRecord SensorViewModel::parseFromPayload(const QByteArray& payload) {
    Q_ASSERT(payload.size() == 6);

    const uint8_t* p = reinterpret_cast<const uint8_t*>(payload.constData());

    uint8_t airHum = p[0];
    int16_t airTmp = bigEndianToInt16(p[1], p[2]);
    uint8_t soilHum = p[3];
    int16_t light = bigEndianToInt16(p[4], p[5]);

    SensorRecord record;
    record.record_time = QDateTime::currentDateTime()
                            .toString("yyyy-MM-dd hh:mm:ss")
                            .toStdString();
    record.air_temp = static_cast<int>(airTmp);
    record.air_humid = static_cast<int>(airHum);
    record.soil_humid = static_cast<int>(soilHum);
    record.light_intensity = static_cast<int>(light);

    qDebug() << "📊 解析传感器数据:"
             << "Temp:" << record.air_temp
             << "AirHum:" << record.air_humid
             << "SoilHum:" << record.soil_humid
             << "Light:" << record.light_intensity;

    return record;
}
//SensorViewModel::SensorViewModel() {}
