#include "ChartViewModel.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <algorithm>
#include <numeric>

ChartViewModel::ChartViewModel(QObject* parent)
    : QObject(parent) {
    qDebug() << "📊 ChartViewModel 初始化完成";
}

ChartViewModel::~ChartViewModel() = default;

// ========================================
// 数据管理
// ========================================

void ChartViewModel::addData(const SensorRecord& data) {
    m_dataRecords.append(data);
    limitDataCount();
    
    emit dataAdded(data);
    emit statisticsUpdated();
    
    qDebug() << "➕ 添加数据点: 总数=" << m_dataRecords.size()
             << "Temp=" << data.air_temp << "°C";
}

void ChartViewModel::clearAllData() {
    m_dataRecords.clear();
    
    emit dataCleared();
    emit statisticsUpdated();
    
    qDebug() << "🗑️ 清空所有数据";
}

QVector<SensorRecord> ChartViewModel::getDataInRange(const QDateTime& startTime, 
                                                       const QDateTime& endTime) const {
    QVector<SensorRecord> result;
    
    for (const auto& record : m_dataRecords) {
        QString timeStr = QString::fromStdString(record.record_time);
        QDateTime recordTime = QDateTime::fromString(timeStr, "yyyy-MM-dd hh:mm:ss");
        
        if (recordTime >= startTime && recordTime <= endTime) {
            result.append(record);
        }
    }
    
    return result;
}

QVector<SensorRecord> ChartViewModel::getLatestData(int count) const {
    if (count >= m_dataRecords.size()) {
        return m_dataRecords;
    }
    
    QVector<SensorRecord> result;
    int startIndex = m_dataRecords.size() - count;
    
    for (int i = startIndex; i < m_dataRecords.size(); ++i) {
        result.append(m_dataRecords[i]);
    }
    
    return result;
}

// ========================================
// 数据统计
// ========================================

double ChartViewModel::getAverageTemperature(int startIndex, int endIndex) const {
    return calculateAverage([](const SensorRecord& r) { return r.air_temp; }, 
                            startIndex, endIndex);
}

double ChartViewModel::getAverageAirHumidity(int startIndex, int endIndex) const {
    return calculateAverage([](const SensorRecord& r) { return r.air_humid; }, 
                            startIndex, endIndex);
}

double ChartViewModel::getAverageSoilHumidity(int startIndex, int endIndex) const {
    return calculateAverage([](const SensorRecord& r) { return r.soil_humid; }, 
                            startIndex, endIndex);
}

double ChartViewModel::getAverageLightIntensity(int startIndex, int endIndex) const {
    return calculateAverage([](const SensorRecord& r) { return r.light_intensity; }, 
                            startIndex, endIndex);
}

void ChartViewModel::getTemperatureRange(int& min, int& max) const {
    calculateRange([](const SensorRecord& r) { return r.air_temp; }, min, max);
}

void ChartViewModel::getAirHumidityRange(int& min, int& max) const {
    calculateRange([](const SensorRecord& r) { return r.air_humid; }, min, max);
}

void ChartViewModel::getSoilHumidityRange(int& min, int& max) const {
    calculateRange([](const SensorRecord& r) { return r.soil_humid; }, min, max);
}

void ChartViewModel::getLightIntensityRange(int& min, int& max) const {
    calculateRange([](const SensorRecord& r) { return r.light_intensity; }, min, max);
}

// ========================================
// 数据导出
// ========================================

bool ChartViewModel::exportToCSV(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "❌ 无法打开文件进行写入:" << filePath;
        return false;
    }
    
    QTextStream out(&file);
    
    // 写入 CSV 头部
    out << "时间,温度(°C),空气湿度(%),土壤湿度(%),光照强度(Lux)\n";
    
    // 写入数据
    for (const auto& record : m_dataRecords) {
        out << QString::fromStdString(record.record_time) << ","
            << record.air_temp << ","
            << record.air_humid << ","
            << record.soil_humid << ","
            << record.light_intensity << "\n";
    }
    
    file.close();
    
    qDebug() << "✅ 数据导出到 CSV:" << filePath << "行数=" << m_dataRecords.size();
    
    return true;
}

bool ChartViewModel::exportToJSON(const QString& filePath) const {
    QJsonArray dataArray;
    
    for (const auto& record : m_dataRecords) {
        QJsonObject obj;
        obj["time"] = QString::fromStdString(record.record_time);
        obj["temperature"] = record.air_temp;
        obj["air_humidity"] = record.air_humid;
        obj["soil_humidity"] = record.soil_humid;
        obj["light_intensity"] = record.light_intensity;
        
        dataArray.append(obj);
    }
    
    QJsonObject root;
    root["data"] = dataArray;
    root["count"] = m_dataRecords.size();
    root["export_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    
    QJsonDocument doc(root);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "❌ 无法打开文件进行写入:" << filePath;
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qDebug() << "✅ 数据导出到 JSON:" << filePath << "行数=" << m_dataRecords.size();
    
    return true;
}

// ========================================
// 数据限制
// ========================================

void ChartViewModel::setMaxDataCount(int maxCount) {
    m_maxDataCount = maxCount;
    limitDataCount();
    
    qDebug() << "📏 设置最大数据点数量:" << maxCount;
}

// ========================================
// 私有辅助函数
// ========================================

void ChartViewModel::limitDataCount() {
    if (m_maxDataCount > 0 && m_dataRecords.size() > m_maxDataCount) {
        int removeCount = m_dataRecords.size() - m_maxDataCount;
        m_dataRecords.remove(0, removeCount);
        
        qDebug() << "✂️ 删除最旧的" << removeCount << "个数据点";
    }
}

template<typename Func>
double ChartViewModel::calculateAverage(Func getValue, int startIndex, int endIndex) const {
    if (m_dataRecords.isEmpty()) {
        return 0.0;
    }
    
    // 处理默认参数
    if (startIndex < 0) startIndex = 0;
    if (endIndex < 0 || endIndex >= m_dataRecords.size()) {
        endIndex = m_dataRecords.size() - 1;
    }
    
    if (startIndex > endIndex) {
        return 0.0;
    }
    
    double sum = 0.0;
    int count = 0;
    
    for (int i = startIndex; i <= endIndex; ++i) {
        sum += getValue(m_dataRecords[i]);
        count++;
    }
    
    return count > 0 ? (sum / count) : 0.0;
}

template<typename Func>
void ChartViewModel::calculateRange(Func getValue, int& min, int& max) const {
    if (m_dataRecords.isEmpty()) {
        min = 0;
        max = 0;
        return;
    }
    
    min = getValue(m_dataRecords[0]);
    max = getValue(m_dataRecords[0]);
    
    for (const auto& record : m_dataRecords) {
        int value = getValue(record);
        if (value < min) min = value;
        if (value > max) max = value;
    }
}
