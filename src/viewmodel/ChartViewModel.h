#ifndef CHARTVIEWMODEL_H
#define CHARTVIEWMODEL_H

#pragma once
#include <QObject>
#include <QVector>
#include <QDateTime>
#include "../model/SensorData.h"

/**
 * @brief 图表数据 ViewModel
 * 
 * 职责：
 * - 管理图表数据的存储
 * - 提供数据的增删查改接口
 * - 数据统计和计算（平均值、最大最小值等）
 * - 数据导出功能
 */
class ChartViewModel : public QObject {
    Q_OBJECT

public:
    explicit ChartViewModel(QObject* parent = nullptr);
    ~ChartViewModel();

    // ========== 数据管理 ==========
    
    /**
     * @brief 添加一条传感器数据
     * @param data 传感器数据记录
     */
    void addData(const SensorRecord& data);
    
    /**
     * @brief 清空所有数据
     */
    void clearAllData();
    
    /**
     * @brief 获取数据点数量
     * @return 数据点总数
     */
    int getDataCount() const { return m_dataRecords.size(); }
    
    /**
     * @brief 获取所有数据记录
     * @return 数据记录列表
     */
    const QVector<SensorRecord>& getAllData() const { return m_dataRecords; }
    
    /**
     * @brief 获取指定范围的数据
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @return 指定时间范围内的数据
     */
    QVector<SensorRecord> getDataInRange(const QDateTime& startTime, const QDateTime& endTime) const;
    
    /**
     * @brief 获取最新的 N 条数据
     * @param count 数据条数
     * @return 最新的 N 条数据
     */
    QVector<SensorRecord> getLatestData(int count) const;

    // ========== 数据统计 ==========
    
    /**
     * @brief 计算温度平均值
     * @param startIndex 开始索引（默认=-1表示所有数据）
     * @param endIndex 结束索引（默认=-1表示所有数据）
     * @return 温度平均值
     */
    double getAverageTemperature(int startIndex = -1, int endIndex = -1) const;
    
    /**
     * @brief 计算空气湿度平均值
     * @param startIndex 开始索引
     * @param endIndex 结束索引
     * @return 空气湿度平均值
     */
    double getAverageAirHumidity(int startIndex = -1, int endIndex = -1) const;
    
    /**
     * @brief 计算土壤湿度平均值
     * @param startIndex 开始索引
     * @param endIndex 结束索引
     * @return 土壤湿度平均值
     */
    double getAverageSoilHumidity(int startIndex = -1, int endIndex = -1) const;
    
    /**
     * @brief 计算光照强度平均值
     * @param startIndex 开始索引
     * @param endIndex 结束索引
     * @return 光照强度平均值
     */
    double getAverageLightIntensity(int startIndex = -1, int endIndex = -1) const;
    
    /**
     * @brief 获取温度最大值和最小值
     * @param min [out] 最小值
     * @param max [out] 最大值
     */
    void getTemperatureRange(int& min, int& max) const;
    
    /**
     * @brief 获取空气湿度最大值和最小值
     * @param min [out] 最小值
     * @param max [out] 最大值
     */
    void getAirHumidityRange(int& min, int& max) const;
    
    /**
     * @brief 获取土壤湿度最大值和最小值
     * @param min [out] 最小值
     * @param max [out] 最大值
     */
    void getSoilHumidityRange(int& min, int& max) const;
    
    /**
     * @brief 获取光照强度最大值和最小值
     * @param min [out] 最小值
     * @param max [out] 最大值
     */
    void getLightIntensityRange(int& min, int& max) const;

    // ========== 数据导出 ==========
    
    /**
     * @brief 导出数据到 CSV 文件
     * @param filePath 文件路径
     * @return true=导出成功, false=导出失败
     */
    bool exportToCSV(const QString& filePath) const;
    
    /**
     * @brief 导出数据到 JSON 文件
     * @param filePath 文件路径
     * @return true=导出成功, false=导出失败
     */
    bool exportToJSON(const QString& filePath) const;

    // ========== 数据限制 ==========
    
    /**
     * @brief 设置最大数据点数量
     * @param maxCount 最大数量（超过时自动删除最旧的数据）
     */
    void setMaxDataCount(int maxCount);
    
    /**
     * @brief 获取最大数据点数量
     * @return 最大数量（-1表示无限制）
     */
    int getMaxDataCount() const { return m_maxDataCount; }

signals:
    /**
     * @brief 新数据添加
     * @param data 新添加的数据
     */
    void dataAdded(const SensorRecord& data);
    
    /**
     * @brief 数据清空
     */
    void dataCleared();
    
    /**
     * @brief 数据统计更新
     */
    void statisticsUpdated();

private:
    QVector<SensorRecord> m_dataRecords;  // 所有数据记录
    int m_maxDataCount = -1;              // 最大数据点数量（-1=无限制）
    
    // 辅助函数：限制数据点数量
    void limitDataCount();
    
    // 辅助函数：计算指定范围的平均值
    template<typename Func>
    double calculateAverage(Func getValue, int startIndex, int endIndex) const;
    
    // 辅助函数：计算指定范围的最大最小值
    template<typename Func>
    void calculateRange(Func getValue, int& min, int& max) const;
};

#endif // CHARTVIEWMODEL_H
