#include "ChartManager.h"
#include <QDebug>
#include <QChartView>

ChartManager::ChartManager(QObject* parent)
    : QObject(parent)
    , m_chart(nullptr)
    , m_temperatureSeries(nullptr)
    , m_airHumiditySeries(nullptr)
    , m_soilHumiditySeries(nullptr)
    , m_sunshineSeries(nullptr)
    , m_axisX(nullptr)
    , m_dateCount(0)
{
    m_startCollectionTime = QDateTime::currentDateTime();
}

ChartManager::~ChartManager() {
    // QChart 会自动管理 series 的内存
}

QChart* ChartManager::createChart() {
    m_chart = new QChart();
    
    setupChart();
    createSeries();
    createAxes();
    
    return m_chart;
}

void ChartManager::setupChart() {
    m_chart->setBackgroundBrush(QColor(245, 245, 245));
    m_chart->setBackgroundRoundness(10);
    m_chart->setTitle("大棚内生态数据实时折线图");
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
}

void ChartManager::createSeries() {
    // 创建土壤湿度折线
    m_soilHumiditySeries = new QLineSeries();
    m_soilHumiditySeries->setName("土壤湿度(%)");
    m_soilHumiditySeries->setColor(Qt::red);
    m_soilHumiditySeries->setPen(QPen(Qt::red, 2));
    
    // 创建空气湿度折线
    m_airHumiditySeries = new QLineSeries();
    m_airHumiditySeries->setName("空气湿度(%)");
    m_airHumiditySeries->setColor(Qt::blue);
    m_airHumiditySeries->setPen(QPen(Qt::blue, 2));
    
    // 创建光照强度折线
    m_sunshineSeries = new QLineSeries();
    m_sunshineSeries->setName("光照强度(Lux)");
    m_sunshineSeries->setColor(QColor(160, 32, 240));
    m_sunshineSeries->setPen(QPen(QColor(160, 32, 240), 2));
    
    // 创建温度折线
    m_temperatureSeries = new QLineSeries();
    m_temperatureSeries->setName("温度(°C)");
    m_temperatureSeries->setColor(Qt::green);
    m_temperatureSeries->setPen(QPen(Qt::green, 2));
    
    // 添加到图表
    m_chart->addSeries(m_soilHumiditySeries);
    m_chart->addSeries(m_airHumiditySeries);
    m_chart->addSeries(m_sunshineSeries);
    m_chart->addSeries(m_temperatureSeries);
}

void ChartManager::createAxes() {
    // 创建时间轴
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("hh:mm:ss");
    m_axisX->setTitleText("时间");
    m_axisX->setTickCount(10);
    m_axisX->setMinorGridLineVisible(true);
    m_axisX->setMinorGridLinePen(QPen(QColor(180, 180, 180), 0.5, Qt::DotLine));
    
    // Y轴 - 土壤湿度
    QValueAxis* axisYSoilHumidity = new QValueAxis();
    axisYSoilHumidity->setRange(0, 100);
    axisYSoilHumidity->setTickCount(11);
    axisYSoilHumidity->setLinePenColor(Qt::red);
    axisYSoilHumidity->setLabelsColor(Qt::red);
    axisYSoilHumidity->setTitleText("土壤湿度(%)");
    
    // Y轴 - 空气湿度
    QValueAxis* axisYAirHumidity = new QValueAxis();
    axisYAirHumidity->setRange(0, 100);
    axisYAirHumidity->setTickCount(11);
    axisYAirHumidity->setLinePenColor(Qt::blue);
    axisYAirHumidity->setLabelsColor(Qt::blue);
    axisYAirHumidity->setTitleText("空气湿度(%)");
    
    // Y轴 - 光照强度
    QValueAxis* axisYSunshine = new QValueAxis();
    axisYSunshine->setRange(0, 2000);
    axisYSunshine->setTickCount(11);
    axisYSunshine->setLinePenColor(QColor(160, 32, 240));
    axisYSunshine->setLabelsColor(QColor(160, 32, 240));
    axisYSunshine->setTitleText("光照强度(Lux)");
    
    // Y轴 - 温度
    QValueAxis* axisYTemperature = new QValueAxis();
    axisYTemperature->setRange(0, 50);
    axisYTemperature->setTickCount(11);
    axisYTemperature->setLinePenColor(Qt::green);
    axisYTemperature->setLabelsColor(Qt::green);
    axisYTemperature->setTitleText("温度(°C)");
    
    // 添加轴到图表
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(axisYSoilHumidity, Qt::AlignLeft);
    m_chart->addAxis(axisYAirHumidity, Qt::AlignLeft);
    m_chart->addAxis(axisYSunshine, Qt::AlignRight);
    m_chart->addAxis(axisYTemperature, Qt::AlignRight);
    
    // 关联折线与轴
    m_soilHumiditySeries->attachAxis(m_axisX);
    m_soilHumiditySeries->attachAxis(axisYSoilHumidity);
    
    m_airHumiditySeries->attachAxis(m_axisX);
    m_airHumiditySeries->attachAxis(axisYAirHumidity);
    
    m_sunshineSeries->attachAxis(m_axisX);
    m_sunshineSeries->attachAxis(axisYSunshine);
    
    m_temperatureSeries->attachAxis(m_axisX);
    m_temperatureSeries->attachAxis(axisYTemperature);
}

void ChartManager::addDataPoint(const SensorRecord& data) {
    // 存储数据
    m_temperatureData.append(data.air_temp);
    m_airhumidityData.append(data.air_humid);
    m_soilhumidityData.append(data.soil_humid);
    m_sunshineData.append(data.light_intensity);
    
    // 获取当前时间
    QDateTime timeNow = QDateTime::currentDateTime();
    m_timeData.append(timeNow);
    
    // 添加数据点到曲线
    qint64 timeMs = timeNow.toMSecsSinceEpoch();
    m_temperatureSeries->append(timeMs, data.air_temp);
    m_airHumiditySeries->append(timeMs, data.air_humid);
    m_soilHumiditySeries->append(timeMs, data.soil_humid);
    m_sunshineSeries->append(timeMs, data.light_intensity);
    
    m_dateCount++;
    
    // 更新时间轴
    updateTimeAxis(timeNow);
    
    // 限制数据点数量
    limitDataPoints();
    
    qDebug() << "ChartManager: 添加数据点" << m_dateCount;
}

void ChartManager::updateTimeAxis(const QDateTime& currentTime) {
    if (m_dateCount > 1) {
        QDateTime minTime = currentTime.addSecs(-TIME_WINDOW_SECONDS);
        QDateTime maxTime = currentTime.addSecs(2);
        m_axisX->setRange(minTime, maxTime);
    }
}

void ChartManager::limitDataPoints() {
    if (m_temperatureSeries->count() > MAX_POINTS) {
        int removeCount = m_temperatureSeries->count() - MAX_POINTS;
        m_temperatureSeries->removePoints(0, removeCount);
        m_airHumiditySeries->removePoints(0, removeCount);
        m_sunshineSeries->removePoints(0, removeCount);
        m_soilHumiditySeries->removePoints(0, removeCount);
    }
}

void ChartManager::clearAllData() {
    // 清除所有曲线数据
    m_temperatureSeries->clear();
    m_airHumiditySeries->clear();
    m_sunshineSeries->clear();
    m_soilHumiditySeries->clear();
    
    // 清空存储的历史数据
    m_timeData.clear();
    m_sunshineData.clear();
    m_soilhumidityData.clear();
    m_airhumidityData.clear();
    m_temperatureData.clear();
    
    // 重置计数器
    m_dateCount = 0;
    m_startCollectionTime = QDateTime::currentDateTime();
    
    qDebug() << "ChartManager: 数据已清除";
}

