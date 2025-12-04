#include "SettingViewModel.h"
#include <QCoreApplication>
#include <QDebug>

SettingViewModel::SettingViewModel(QObject* parent)
    : QObject(parent) {
    // 初始化 QSettings（使用 ini 文件格式）
    m_settings = new QSettings(
        QCoreApplication::applicationDirPath() + "/greenhouse_settings.ini",
        QSettings::IniFormat,
        this
    );
    
    // 加载设置
    loadSettings();
    
    qDebug() << "⚙️ SettingViewModel 初始化完成";
    qDebug() << "📂 设置文件位置:" << m_settings->fileName();
}

SettingViewModel::~SettingViewModel() {
    saveSettings();
}

// ========================================
// 阈值设置
// ========================================

int SettingViewModel::getFanOnThreshold() const {
    return m_settings->value("threshold/fan_on", DEFAULT_FAN_ON).toInt();
}

void SettingViewModel::setFanOnThreshold(int value) {
    m_settings->setValue("threshold/fan_on", value);
    emit thresholdChanged();
    qDebug() << "⚙️ 设置风扇开启阈值:" << value << "°C";
}

int SettingViewModel::getFanOffThreshold() const {
    return m_settings->value("threshold/fan_off", DEFAULT_FAN_OFF).toInt();
}

void SettingViewModel::setFanOffThreshold(int value) {
    m_settings->setValue("threshold/fan_off", value);
    emit thresholdChanged();
    qDebug() << "⚙️ 设置风扇关闭阈值:" << value << "°C";
}

int SettingViewModel::getPumpOnThreshold() const {
    return m_settings->value("threshold/pump_on", DEFAULT_PUMP_ON).toInt();
}

void SettingViewModel::setPumpOnThreshold(int value) {
    m_settings->setValue("threshold/pump_on", value);
    emit thresholdChanged();
    qDebug() << "⚙️ 设置水泵开启阈值:" << value << "%";
}

int SettingViewModel::getPumpOffThreshold() const {
    return m_settings->value("threshold/pump_off", DEFAULT_PUMP_OFF).toInt();
}

void SettingViewModel::setPumpOffThreshold(int value) {
    m_settings->setValue("threshold/pump_off", value);
    emit thresholdChanged();
    qDebug() << "⚙️ 设置水泵关闭阈值:" << value << "%";
}

int SettingViewModel::getLampOnThreshold() const {
    return m_settings->value("threshold/lamp_on", DEFAULT_LAMP_ON).toInt();
}

void SettingViewModel::setLampOnThreshold(int value) {
    m_settings->setValue("threshold/lamp_on", value);
    emit thresholdChanged();
    qDebug() << "⚙️ 设置灯光开启阈值:" << value << " Lux";
}

int SettingViewModel::getLampOffThreshold() const {
    return m_settings->value("threshold/lamp_off", DEFAULT_LAMP_OFF).toInt();
}

void SettingViewModel::setLampOffThreshold(int value) {
    m_settings->setValue("threshold/lamp_off", value);
    emit thresholdChanged();
    qDebug() << "⚙️ 设置灯光关闭阈值:" << value << " Lux";
}

// ========================================
// 串口设置
// ========================================

QString SettingViewModel::getLastSerialPort() const {
    return m_settings->value("serial/last_port", "").toString();
}

void SettingViewModel::setLastSerialPort(const QString& portName) {
    m_settings->setValue("serial/last_port", portName);
    emit serialSettingsChanged();
    qDebug() << "⚙️ 保存上次使用的串口:" << portName;
}

int SettingViewModel::getSerialBaudRate() const {
    return m_settings->value("serial/baud_rate", DEFAULT_BAUD_RATE).toInt();
}

void SettingViewModel::setSerialBaudRate(int baudRate) {
    m_settings->setValue("serial/baud_rate", baudRate);
    emit serialSettingsChanged();
    qDebug() << "⚙️ 设置串口波特率:" << baudRate;
}

// ========================================
// 图表设置
// ========================================

int SettingViewModel::getChartMaxPoints() const {
    return m_settings->value("chart/max_points", DEFAULT_CHART_MAX_POINTS).toInt();
}

void SettingViewModel::setChartMaxPoints(int count) {
    m_settings->setValue("chart/max_points", count);
    emit chartSettingsChanged();
    qDebug() << "⚙️ 设置图表最大数据点:" << count;
}

int SettingViewModel::getChartTimeWindow() const {
    return m_settings->value("chart/time_window", DEFAULT_CHART_TIME_WINDOW).toInt();
}

void SettingViewModel::setChartTimeWindow(int seconds) {
    m_settings->setValue("chart/time_window", seconds);
    emit chartSettingsChanged();
    qDebug() << "⚙️ 设置图表时间窗口:" << seconds << "秒";
}

bool SettingViewModel::getChartAntialiasing() const {
    return m_settings->value("chart/antialiasing", true).toBool();
}

void SettingViewModel::setChartAntialiasing(bool enabled) {
    m_settings->setValue("chart/antialiasing", enabled);
    emit chartSettingsChanged();
    qDebug() << "⚙️ 设置图表抗锯齿:" << (enabled ? "启用" : "禁用");
}

// ========================================
// 数据采集设置
// ========================================

int SettingViewModel::getDataCollectionInterval() const {
    return m_settings->value("data/collection_interval", DEFAULT_DATA_INTERVAL).toInt();
}

void SettingViewModel::setDataCollectionInterval(int seconds) {
    m_settings->setValue("data/collection_interval", seconds);
    emit dataCollectionSettingsChanged();
    qDebug() << "⚙️ 设置数据采集间隔:" << seconds << "秒";
}

bool SettingViewModel::getAutoSaveToDatabase() const {
    return m_settings->value("data/auto_save", true).toBool();
}

void SettingViewModel::setAutoSaveToDatabase(bool enabled) {
    m_settings->setValue("data/auto_save", enabled);
    emit dataCollectionSettingsChanged();
    qDebug() << "⚙️ 设置自动保存到数据库:" << (enabled ? "启用" : "禁用");
}

// ========================================
// 通用设置
// ========================================

void SettingViewModel::resetToDefaults() {
    qDebug() << "🔄 重置所有设置为默认值";
    
    // 阈值
    setFanOnThreshold(DEFAULT_FAN_ON);
    setFanOffThreshold(DEFAULT_FAN_OFF);
    setPumpOnThreshold(DEFAULT_PUMP_ON);
    setPumpOffThreshold(DEFAULT_PUMP_OFF);
    setLampOnThreshold(DEFAULT_LAMP_ON);
    setLampOffThreshold(DEFAULT_LAMP_OFF);
    
    // 串口
    setSerialBaudRate(DEFAULT_BAUD_RATE);
    
    // 图表
    setChartMaxPoints(DEFAULT_CHART_MAX_POINTS);
    setChartTimeWindow(DEFAULT_CHART_TIME_WINDOW);
    setChartAntialiasing(true);
    
    // 数据采集
    setDataCollectionInterval(DEFAULT_DATA_INTERVAL);
    setAutoSaveToDatabase(true);
    
    saveSettings();
    
    qDebug() << "✅ 设置重置完成";
}

void SettingViewModel::saveSettings() {
    m_settings->sync();
    qDebug() << "💾 设置已保存";
}

void SettingViewModel::loadSettings() {
    qDebug() << "📂 加载设置...";
    
    // 设置已经自动加载，这里只是触发信号
    emit thresholdChanged();
    emit serialSettingsChanged();
    emit chartSettingsChanged();
    emit dataCollectionSettingsChanged();
    
    // 打印当前设置
    qDebug() << "📋 当前设置:";
    qDebug() << "  风扇阈值:" << getFanOffThreshold() << "-" << getFanOnThreshold() << "°C";
    qDebug() << "  水泵阈值:" << getPumpOffThreshold() << "-" << getPumpOnThreshold() << "%";
    qDebug() << "  灯光阈值:" << getLampOffThreshold() << "-" << getLampOnThreshold() << " Lux";
    qDebug() << "  串口波特率:" << getSerialBaudRate();
    qDebug() << "  图表最大点数:" << getChartMaxPoints();
    qDebug() << "  数据采集间隔:" << getDataCollectionInterval() << "秒";
}
