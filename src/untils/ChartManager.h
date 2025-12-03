#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QVector>
#include "../model/SensorData.h"

QT_CHARTS_USE_NAMESPACE

/**
 * @brief 图表管理器 - 负责所有图表相关的操作
 * 职责：
 * - 初始化图表和曲线
 * - 更新图表数据
 * - 管理数据点
 * - 控制显示范围
 */
class ChartManager : public QObject {
    Q_OBJECT

public:
    explicit ChartManager(QObject* parent = nullptr);
    ~ChartManager();

    // 初始化图表
    QChart* createChart();
    
    // 数据更新
    void addDataPoint(const SensorRecord& data);
    void clearAllData();
    
    // 获取数据
    int getDataCount() const { return m_dateCount; }
    const QVector<double>& getTemperatureData() const { return m_temperatureData; }
    const QVector<double>& getAirHumidityData() const { return m_airhumidityData; }
    const QVector<double>& getSoilHumidityData() const { return m_soilhumidityData; }
    const QVector<double>& getSunshineData() const { return m_sunshineData; }

private:
    // 初始化辅助函数
    void createSeries();
    void createAxes();
    void setupChart();
    
    // 数据管理
    void updateTimeAxis(const QDateTime& currentTime);
    void limitDataPoints();

    // 图表组件
    QChart* m_chart;
    QLineSeries* m_temperatureSeries;
    QLineSeries* m_airHumiditySeries;
    QLineSeries* m_soilHumiditySeries;
    QLineSeries* m_sunshineSeries;
    QDateTimeAxis* m_axisX;
    
    // 数据存储
    QVector<QDateTime> m_timeData;
    QVector<double> m_temperatureData;
    QVector<double> m_airhumidityData;
    QVector<double> m_soilhumidityData;
    QVector<double> m_sunshineData;
    
    int m_dateCount;
    QDateTime m_startCollectionTime;
    
    // 常量
    static const int MAX_POINTS = 100;
    static const int TIME_WINDOW_SECONDS = 30;
};

#endif // CHARTMANAGER_H

