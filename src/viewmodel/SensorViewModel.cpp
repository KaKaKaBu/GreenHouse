#include "SensorViewModel.h"
#include <QDateTime>
#include <QDebug>

SensorViewModel::SensorViewModel(QObject* parent)
    : QObject(parent) {
    qDebug() << "🌡️ SensorViewModel 初始化完成";
}

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
             << "Temp:" << record.air_temp << "°C"
             << "AirHum:" << record.air_humid << "%"
             << "SoilHum:" << record.soil_humid << "%"
             << "Light:" << record.light_intensity << "Lux";

    return record;
}

// ========================================
// 数据验证
// ========================================

bool SensorViewModel::validateSensorData(const SensorRecord& record) {
    bool tempValid = isValidTemperature(record.air_temp);
    bool airHumValid = isValidHumidity(record.air_humid);
    bool soilHumValid = isValidHumidity(record.soil_humid);
    bool lightValid = isValidLightIntensity(record.light_intensity);
    
    if (!tempValid) {
        qWarning() << "⚠️ 温度数据异常:" << record.air_temp << "°C";
    }
    if (!airHumValid) {
        qWarning() << "⚠️ 空气湿度数据异常:" << record.air_humid << "%";
    }
    if (!soilHumValid) {
        qWarning() << "⚠️ 土壤湿度数据异常:" << record.soil_humid << "%";
    }
    if (!lightValid) {
        qWarning() << "⚠️ 光照强度数据异常:" << record.light_intensity << "Lux";
    }
    
    return tempValid && airHumValid && soilHumValid && lightValid;
}

bool SensorViewModel::isValidTemperature(int temperature) {
    return temperature >= MIN_TEMPERATURE && temperature <= MAX_TEMPERATURE;
}

bool SensorViewModel::isValidHumidity(int humidity) {
    return humidity >= MIN_HUMIDITY && humidity <= MAX_HUMIDITY;
}

bool SensorViewModel::isValidLightIntensity(int lightIntensity) {
    return lightIntensity >= MIN_LIGHT && lightIntensity <= MAX_LIGHT;
}

// ========================================
// 数据格式化
// ========================================

QString SensorViewModel::formatTemperature(int temperature) {
    return QString("%1°C").arg(temperature);
}

QString SensorViewModel::formatHumidity(int humidity) {
    return QString("%1%").arg(humidity);
}

QString SensorViewModel::formatLightIntensity(int lightIntensity) {
    return QString("%1 Lux").arg(lightIntensity);
}

// ========================================
// 数据分析
// ========================================

QString SensorViewModel::getTemperatureLevel(int temperature) {
    if (temperature < 15) return "偏低";
    if (temperature < 20) return "较低";
    if (temperature <= 28) return "适宜";
    if (temperature <= 35) return "较高";
    return "偏高";
}

QString SensorViewModel::getHumidityLevel(int humidity) {
    if (humidity < 30) return "干燥";
    if (humidity < 40) return "较干燥";
    if (humidity <= 70) return "适宜";
    if (humidity <= 80) return "较潮湿";
    return "潮湿";
}

QString SensorViewModel::getLightLevel(int lightIntensity) {
    if (lightIntensity < 100) return "昏暗";
    if (lightIntensity < 300) return "较暗";
    if (lightIntensity <= 1000) return "适宜";
    if (lightIntensity <= 3000) return "较亮";
    return "强光";
}
