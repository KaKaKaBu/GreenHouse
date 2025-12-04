#ifndef SETTINGVIEWMODEL_H
#define SETTINGVIEWMODEL_H

#pragma once
#include <QObject>
#include <QString>
#include <QSettings>

/**
 * @brief 设置管理 ViewModel
 * 
 * 职责：
 * - 管理应用程序设置
 * - 提供设置的读写接口
 * - 阈值配置管理
 * - 用户偏好设置
 */
class SettingViewModel : public QObject {
    Q_OBJECT

public:
    explicit SettingViewModel(QObject* parent = nullptr);
    ~SettingViewModel();

    // ========== 阈值设置 ==========
    
    /**
     * @brief 获取风扇开启温度阈值
     * @return 温度值（°C）
     */
    int getFanOnThreshold() const;
    
    /**
     * @brief 设置风扇开启温度阈值
     * @param value 温度值（°C）
     */
    void setFanOnThreshold(int value);
    
    /**
     * @brief 获取风扇关闭温度阈值
     * @return 温度值（°C）
     */
    int getFanOffThreshold() const;
    
    /**
     * @brief 设置风扇关闭温度阈值
     * @param value 温度值（°C）
     */
    void setFanOffThreshold(int value);
    
    /**
     * @brief 获取水泵开启土壤湿度阈值
     * @return 湿度值（%）
     */
    int getPumpOnThreshold() const;
    
    /**
     * @brief 设置水泵开启土壤湿度阈值
     * @param value 湿度值（%）
     */
    void setPumpOnThreshold(int value);
    
    /**
     * @brief 获取水泵关闭土壤湿度阈值
     * @return 湿度值（%）
     */
    int getPumpOffThreshold() const;
    
    /**
     * @brief 设置水泵关闭土壤湿度阈值
     * @param value 湿度值（%）
     */
    void setPumpOffThreshold(int value);
    
    /**
     * @brief 获取灯光开启光照阈值
     * @return 光照值（Lux）
     */
    int getLampOnThreshold() const;
    
    /**
     * @brief 设置灯光开启光照阈值
     * @param value 光照值（Lux）
     */
    void setLampOnThreshold(int value);
    
    /**
     * @brief 获取灯光关闭光照阈值
     * @return 光照值（Lux）
     */
    int getLampOffThreshold() const;
    
    /**
     * @brief 设置灯光关闭光照阈值
     * @param value 光照值（Lux）
     */
    void setLampOffThreshold(int value);

    // ========== 串口设置 ==========
    
    /**
     * @brief 获取上次使用的串口名称
     * @return 串口名称
     */
    QString getLastSerialPort() const;
    
    /**
     * @brief 设置上次使用的串口名称
     * @param portName 串口名称
     */
    void setLastSerialPort(const QString& portName);
    
    /**
     * @brief 获取串口波特率
     * @return 波特率
     */
    int getSerialBaudRate() const;
    
    /**
     * @brief 设置串口波特率
     * @param baudRate 波特率
     */
    void setSerialBaudRate(int baudRate);

    // ========== 图表设置 ==========
    
    /**
     * @brief 获取图表最大数据点数量
     * @return 数据点数量
     */
    int getChartMaxPoints() const;
    
    /**
     * @brief 设置图表最大数据点数量
     * @param count 数据点数量
     */
    void setChartMaxPoints(int count);
    
    /**
     * @brief 获取图表时间窗口（秒）
     * @return 时间窗口秒数
     */
    int getChartTimeWindow() const;
    
    /**
     * @brief 设置图表时间窗口（秒）
     * @param seconds 时间窗口秒数
     */
    void setChartTimeWindow(int seconds);
    
    /**
     * @brief 获取图表是否启用抗锯齿
     * @return true=启用, false=禁用
     */
    bool getChartAntialiasing() const;
    
    /**
     * @brief 设置图表是否启用抗锯齿
     * @param enabled true=启用, false=禁用
     */
    void setChartAntialiasing(bool enabled);

    // ========== 数据采集设置 ==========
    
    /**
     * @brief 获取数据采集间隔（秒）
     * @return 采集间隔秒数
     */
    int getDataCollectionInterval() const;
    
    /**
     * @brief 设置数据采集间隔（秒）
     * @param seconds 采集间隔秒数
     */
    void setDataCollectionInterval(int seconds);
    
    /**
     * @brief 获取是否自动保存数据到数据库
     * @return true=自动保存, false=不自动保存
     */
    bool getAutoSaveToDatabase() const;
    
    /**
     * @brief 设置是否自动保存数据到数据库
     * @param enabled true=自动保存, false=不自动保存
     */
    void setAutoSaveToDatabase(bool enabled);

    // ========== 通用设置 ==========
    
    /**
     * @brief 重置所有设置为默认值
     */
    void resetToDefaults();
    
    /**
     * @brief 保存所有设置
     */
    void saveSettings();
    
    /**
     * @brief 加载所有设置
     */
    void loadSettings();

signals:
    /**
     * @brief 阈值设置变化
     */
    void thresholdChanged();
    
    /**
     * @brief 串口设置变化
     */
    void serialSettingsChanged();
    
    /**
     * @brief 图表设置变化
     */
    void chartSettingsChanged();
    
    /**
     * @brief 数据采集设置变化
     */
    void dataCollectionSettingsChanged();

private:
    QSettings* m_settings;
    
    // 默认值常量
    static constexpr int DEFAULT_FAN_ON = 30;
    static constexpr int DEFAULT_FAN_OFF = 25;
    static constexpr int DEFAULT_PUMP_ON = 30;
    static constexpr int DEFAULT_PUMP_OFF = 50;
    static constexpr int DEFAULT_LAMP_ON = 200;
    static constexpr int DEFAULT_LAMP_OFF = 500;
    static constexpr int DEFAULT_BAUD_RATE = 9600;
    static constexpr int DEFAULT_CHART_MAX_POINTS = 100;
    static constexpr int DEFAULT_CHART_TIME_WINDOW = 300;  // 5分钟
    static constexpr int DEFAULT_DATA_INTERVAL = 10;  // 10秒
};

#endif // SETTINGVIEWMODEL_H
