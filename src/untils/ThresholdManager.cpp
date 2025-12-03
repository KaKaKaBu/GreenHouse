#include "ThresholdManager.h"
#include <QDebug>

ThresholdManager::ThresholdManager(QObject* parent)
    : QObject(parent)
    , m_serialViewModel(nullptr)
{
}

void ThresholdManager::setSerialViewModel(SerialViewModel* viewModel) {
    m_serialViewModel = viewModel;
}

bool ThresholdManager::validateThreshold(int high, int low, int min, int max) {
    if (low < min || high > max) {
        emit validationError(QString("阈值超出范围 [%1, %2]").arg(min).arg(max));
        return false;
    }
    
    if (high <= low) {
        emit validationError("高阈值必须大于低阈值");
        return false;
    }
    
    return true;
}

void ThresholdManager::setTemperatureThreshold(int high, int low) {
    if (!validateThreshold(high, low, 0, 50)) {
        return;
    }
    
    m_thresholds.temperatureHigh = high;
    m_thresholds.temperatureLow = low;
    
    emit thresholdsChanged();
    qDebug() << "ThresholdManager: 温度阈值设置为" << low << "~" << high;
}

void ThresholdManager::setAirHumidityThreshold(int high, int low) {
    if (!validateThreshold(high, low, 0, 100)) {
        return;
    }
    
    m_thresholds.airHumidityHigh = high;
    m_thresholds.airHumidityLow = low;
    
    emit thresholdsChanged();
    qDebug() << "ThresholdManager: 空气湿度阈值设置为" << low << "~" << high;
}

void ThresholdManager::setLightIntensityThreshold(int high, int low) {
    if (!validateThreshold(high, low, 0, 2000)) {
        return;
    }
    
    m_thresholds.lightIntensityHigh = high;
    m_thresholds.lightIntensityLow = low;
    
    emit thresholdsChanged();
    qDebug() << "ThresholdManager: 光照强度阈值设置为" << low << "~" << high;
}

void ThresholdManager::setSoilMoistureThreshold(int high, int low) {
    if (!validateThreshold(high, low, 0, 100)) {
        return;
    }
    
    m_thresholds.soilMoistureHigh = high;
    m_thresholds.soilMoistureLow = low;
    
    emit thresholdsChanged();
    qDebug() << "ThresholdManager: 土壤湿度阈值设置为" << low << "~" << high;
}

void ThresholdManager::sendThresholdsToDevice() {
    if (!m_serialViewModel) {
        qDebug() << "ThresholdManager: SerialViewModel 未设置";
        return;
    }
    
    // 注意：这里简化为只发送温度和湿度阈值
    // 根据下位机协议，需要发送6个字节的阈值数据
    m_serialViewModel->sendThreshold(
        m_thresholds.temperatureHigh,
        m_thresholds.temperatureLow,
        m_thresholds.soilMoistureHigh,
        m_thresholds.soilMoistureLow,
        m_thresholds.lightIntensityHigh,
        m_thresholds.lightIntensityLow
    );
    
    qDebug() << "ThresholdManager: 阈值已发送到下位机";
}

