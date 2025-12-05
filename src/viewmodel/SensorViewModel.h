#ifndef SENSORVIEWMODEL_H
#define SENSORVIEWMODEL_H
// viewmodel/SensorViewModel.h
#pragma once
#include <QObject>
#include "../model/SensorData.h"

/**
 * @brief 传感器数据 ViewModel
 * 
 * 职责：
 * - 解析下位机传感器数据
 * - 验证数据有效性
 * - 数据格式转换
 * - 数据异常检测
 */
class SensorViewModel : public QObject {
    Q_OBJECT

public:
    explicit SensorViewModel(QObject* parent = nullptr);

    /**
     * @brief 将下位机的 6 字节 payload 解析为 SensorRecord
     * @param payload 必须是 6 字节（[airHum, tmpH, tmpL, soilHum, lightH, lightL]）
     * @return 解析成功的 SensorRecord（时间由调用者设置）
     */
    static SensorRecord parseFromPayload(const QByteArray& payload);

    // ========== 数据验证 ==========
    
    /**
     * @brief 验证传感器数据是否有效
     * @param record 传感器数据记录
     * @return true=数据有效, false=数据异常
     */
    static bool validateSensorData(const SensorRecord& record);
    
    /**
     * @brief 验证温度值是否在合理范围内
     * @param temperature 温度值（°C）
     * @return true=合理, false=异常
     */
    static bool isValidTemperature(int temperature);
    
    /**
     * @brief 验证湿度值是否在合理范围内
     * @param humidity 湿度值（%）
     * @return true=合理, false=异常
     */
    static bool isValidHumidity(int humidity);
    
    /**
     * @brief 验证光照强度是否在合理范围内
     * @param lightIntensity 光照强度（Lux）
     * @return true=合理, false=异常
     */
    static bool isValidLightIntensity(int lightIntensity);

    // ========== 数据格式化 ==========
    
    /**
     * @brief 格式化温度显示
     * @param temperature 温度值
     * @return 格式化的字符串（如 "23.5°C"）
     */
    static QString formatTemperature(int temperature);
    
    /**
     * @brief 格式化湿度显示
     * @param humidity 湿度值
     * @return 格式化的字符串（如 "65%"）
     */
    static QString formatHumidity(int humidity);
    
    /**
     * @brief 格式化光照强度显示
     * @param lightIntensity 光照强度
     * @return 格式化的字符串（如 "1200 Lux"）
     */
    static QString formatLightIntensity(int lightIntensity);

    // ========== 数据分析 ==========
    
    /**
     * @brief 获取温度等级描述
     * @param temperature 温度值
     * @return 描述字符串（如 "适宜"、"偏高"、"偏低"）
     */
    static QString getTemperatureLevel(int temperature);
    
    /**
     * @brief 获取湿度等级描述
     * @param humidity 湿度值
     * @return 描述字符串
     */
    static QString getHumidityLevel(int humidity);
    
    /**
     * @brief 获取光照等级描述
     * @param lightIntensity 光照强度
     * @return 描述字符串
     */
    static QString getLightLevel(int lightIntensity);

signals:
    /**
     * @brief 传感器数据更新
     * @param record 新的传感器数据
     */
    void sensorDataUpdated(const SensorRecord& record);
    
    /**
     * @brief 检测到异常数据
     * @param record 异常的数据
     * @param reason 异常原因
     */
    void abnormalDataDetected(const SensorRecord& record, const QString& reason);

private:
    // 数据有效范围常量
    static constexpr int MIN_TEMPERATURE = -20;  // 最低温度（°C）
    static constexpr int MAX_TEMPERATURE = 60;   // 最高温度（°C）
    static constexpr int MIN_HUMIDITY = 0;       // 最低湿度（%）
    static constexpr int MAX_HUMIDITY = 100;     // 最高湿度（%）
    static constexpr int MIN_LIGHT = 0;          // 最低光照（%）
    static constexpr int MAX_LIGHT = 100;      // 最高光照（%）
};

#endif // SENSORVIEWMODEL_H
