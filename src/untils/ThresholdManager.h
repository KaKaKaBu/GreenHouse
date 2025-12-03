#ifndef THRESHOLDMANAGER_H
#define THRESHOLDMANAGER_H

#include <QObject>
#include "../viewmodel/SerialViewModel.h"

/**
 * @brief 阈值管理器 - 负责阈值的管理和同步
 * 职责：
 * - 存储阈值数据
 * - 验证阈值合法性
 * - 发送阈值到下位机
 * - 提供阈值查询接口
 */
class ThresholdManager : public QObject {
    Q_OBJECT

public:
    struct ThresholdData {
        int temperatureHigh = 30;
        int temperatureLow = 20;
        int airHumidityHigh = 70;
        int airHumidityLow = 50;
        int lightIntensityHigh = 800;
        int lightIntensityLow = 300;
        int soilMoistureHigh = 70;
        int soilMoistureLow = 40;
    };

    explicit ThresholdManager(QObject* parent = nullptr);
    
    // 设置 SerialViewModel
    void setSerialViewModel(SerialViewModel* viewModel);
    
    // 设置阈值
    void setTemperatureThreshold(int high, int low);
    void setAirHumidityThreshold(int high, int low);
    void setLightIntensityThreshold(int high, int low);
    void setSoilMoistureThreshold(int high, int low);
    
    // 获取阈值
    const ThresholdData& getThresholds() const { return m_thresholds; }
    
    // 验证阈值
    bool validateThreshold(int high, int low, int min, int max);
    
    // 发送阈值到下位机
    void sendThresholdsToDevice();

signals:
    void thresholdsChanged();
    void validationError(const QString& message);

private:
    SerialViewModel* m_serialViewModel;
    ThresholdData m_thresholds;
};

#endif // THRESHOLDMANAGER_H

