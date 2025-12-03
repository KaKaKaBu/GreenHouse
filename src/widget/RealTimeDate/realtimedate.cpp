//
// Created by Trubson on 2025/12/3.
// Refactored version with better separation of concerns
//

#include "../../../realtimedate_refactored.h"
#include "ui_RealTimeDate.h"

#include <QVBoxLayout>
#include <QChartView>
#include <QWheelEvent>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

// ========================================
// 自定义 ChartView - 支持鼠标滚轮缩放
// ========================================
class CustomChartView : public QChartView {
public:
    CustomChartView(QChart *chart, QWidget *parent = nullptr) 
        : QChartView(chart, parent) {
        setRubberBand(QChartView::RectangleRubberBand);
        setRenderHint(QPainter::Antialiasing);
        setMouseTracking(true);
        setDragMode(QChartView::ScrollHandDrag);
    }

protected:
    void wheelEvent(QWheelEvent *event) override {
        QChart *chart = this->chart();
        int delta = event->angleDelta().y();
        qreal factor = delta > 0 ? 1.1 : 0.9;
        chart->zoom(factor);
        QChartView::wheelEvent(event);
    }
};

// ========================================
// 构造函数
// ========================================
RealTimeDate::RealTimeDate(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RealTimeDate)
    , m_isCollecting(false)
    , m_isUpdatingSlider(false)
    , m_isUpdatingLineEdit(false)
{
    ui->setupUi(this);
    
    // 加载样式表
    loadStyleSheet();
    
    // 创建管理器
    m_chartManager = new ChartManager(this);
    m_serialManager = new SerialConnectionManager(this);
    m_deviceController = new DeviceController(this);
    m_thresholdManager = new ThresholdManager(this);
    
    // 初始化图表
    QChart* chart = m_chartManager->createChart();
    CustomChartView* chartView = new CustomChartView(chart);
    
    // 将图表添加到 frame_2
    QVBoxLayout* layout = new QVBoxLayout(ui->frame_2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(chartView);
    ui->frame_2->setLayout(layout);
    
    // 初始化串口列表
    QStringList ports = m_serialManager->getAvailablePorts();
    ui->cbxSerial->clear();
    ui->cbxSerial->addItems(ports);
    
    // 初始化设备按钮状态
    updateDeviceButtonsUI();
    
    // 连接信号槽
    connectSignals();
    
    qDebug() << "RealTimeDate (Refactored): 初始化完成";
}

RealTimeDate::~RealTimeDate() {
    delete ui;
}

// ========================================
// 信号槽连接
// ========================================
void RealTimeDate::connectSignals() {
    // 设备控制器信号
    connect(m_deviceController, &DeviceController::fanStateChanged,
            this, &RealTimeDate::onFanStateChanged);
    connect(m_deviceController, &DeviceController::lightStateChanged,
            this, &RealTimeDate::onLightStateChanged);
    connect(m_deviceController, &DeviceController::pumpStateChanged,
            this, &RealTimeDate::onPumpStateChanged);
    connect(m_deviceController, &DeviceController::autoModeChanged,
            this, &RealTimeDate::onAutoModeChanged);
    
    // 串口管理器信号
    connect(m_serialManager, &SerialConnectionManager::connected,
            this, [this](const QString& port) {
        qDebug() << "已连接到" << port;
    });
    
    connect(m_serialManager, &SerialConnectionManager::disconnected,
            this, [this]() {
        qDebug() << "串口已断开";
        m_isCollecting = false;
    });
}

// ========================================
// 样式表加载
// ========================================
void RealTimeDate::loadStyleSheet() {
    QFile styleFile(":/widget/RealTimeDate/realtimedate.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "样式表加载成功";
    } else {
        qDebug() << "样式表加载失败";
    }
}

// ========================================
// 串口连接
// ========================================
void RealTimeDate::on_pbtlink_clicked() {
    if (!m_serialManager->isConnected()) {
        // ========== 连接串口 ==========
        QString portName = ui->cbxSerial->currentText();
        
        if (portName.isEmpty()) {
            QMessageBox::warning(this, "串口选择错误", 
                "请先在下拉框中选择一个串口！\n\n"
                "提示：如果没有可用串口，请检查：\n"
                "1. Arduino是否已通过USB连接\n"
                "2. 驱动是否已正确安装\n"
                "3. 设备管理器中是否显示COM口");
            return;
        }
        
        if (m_serialManager->connectToPort(portName)) {
            // 连接成功
            SerialViewModel* vm = m_serialManager->getSerialViewModel();
            
            // 设置到控制器
            m_deviceController->setSerialViewModel(vm);
            m_thresholdManager->setSerialViewModel(vm);
            
            // 连接数据接收信号
            connect(vm, &SerialViewModel::sensorDataReceived,
                    this, &RealTimeDate::onSensorDataReceived);
            connect(vm, &SerialViewModel::actuatorStateReceived,
                    this, &RealTimeDate::onActuatorStateReceived);
            connect(vm, &SerialViewModel::timeWeatherReceived,
                    this, &RealTimeDate::onTimeWeatherReceived);
            
            // 更新UI
            ui->pbtlink->setText("断开");
            
            QMessageBox::information(this, "连接成功", 
                QString("串口 %1 连接成功！\n\n"
                        "波特率: 9600\n"
                        "数据位: 8\n"
                        "停止位: 1\n"
                        "校验位: 无\n\n"
                        "现在可以点击\"开始采集\"查看实时数据。").arg(portName));
            
        } else {
            // 连接失败
            QMessageBox::critical(this, "串口连接失败", 
                QString("无法打开串口 %1\n\n"
                        "错误信息：%2\n\n"
                        "可能的原因：\n"
                        "1. 串口被其他程序占用（如Arduino IDE）\n"
                        "2. 没有访问串口的权限\n"
                        "3. 设备已断开连接\n\n"
                        "请关闭占用串口的程序后重试。")
                        .arg(portName)
                        .arg(m_serialManager->getLastError()));
        }
        
    } else {
        // ========== 断开串口 ==========
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认断开", 
            "确定要断开串口连接吗？\n\n"
            "断开后将停止数据采集。",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            m_serialManager->disconnect();
            m_isCollecting = false;
            ui->pbtlink->setText("连接");
            QMessageBox::information(this, "已断开", "串口已断开连接。");
        }
    }
}

// ========================================
// 数据采集控制
// ========================================
void RealTimeDate::on_pbtStart_clicked() {
    if (!m_serialManager->isConnected()) {
        QMessageBox::warning(this, "无法开始采集", 
            "请先连接串口！\n\n"
            "操作步骤：\n"
            "1. 在\"串口选择\"下拉框中选择COM口\n"
            "2. 点击\"连接\"按钮建立连接\n"
            "3. 再点击\"开始采集\"按钮");
        return;
    }
    
    if (!m_isCollecting) {
        m_isCollecting = true;
        
        QMessageBox::information(this, "采集已开始", 
            "数据采集已开始！\n\n"
            "右侧图表将实时显示传感器数据：\n"
            "• 绿色曲线：温度（°C）\n"
            "• 蓝色曲线：空气湿度（%）\n"
            "• 红色曲线：土壤湿度（%）\n"
            "• 紫色曲线：光照强度（Lux）\n\n"
            "数据更新间隔：约10秒");
        
        qDebug() << "开始数据采集";
    } else {
        QMessageBox::information(this, "提示", 
            "数据采集已经在运行中！\n\n"
            "如需停止采集，请点击\"结束采集\"按钮。");
    }
}

void RealTimeDate::on_pbtEnd_clicked() {
    if (m_isCollecting) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认停止", 
            "确定要停止数据采集吗？\n\n"
            "停止后将不再接收新的传感器数据，\n"
            "但已采集的数据仍会保留在图表中。",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            m_isCollecting = false;
            QMessageBox::information(this, "已停止", 
                "数据采集已停止。\n\n"
                "如需继续采集，请点击\"开始采集\"按钮。");
            qDebug() << "停止数据采集";
        }
    } else {
        QMessageBox::information(this, "提示", 
            "当前没有正在进行的数据采集。\n\n"
            "请先点击\"开始采集\"按钮。");
    }
}

void RealTimeDate::on_pbtClaer_clicked() {
    int dataCount = m_chartManager->getDataCount();
    
    if (dataCount == 0) {
        QMessageBox::information(this, "提示", "当前没有数据需要清除。");
        return;
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "确认清除", 
        QString("确定要清除所有数据吗？\n\n"
                "将清除：\n"
                "• 图表中的所有数据点（共%1个）\n"
                "• 所有历史记录\n\n"
                "此操作不可恢复！").arg(dataCount),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_chartManager->clearAllData();
        QMessageBox::information(this, "清除成功", 
            "所有数据已清除！\n\n"
            "图表已重置，可以开始新的数据采集。");
        qDebug() << "数据已清除";
    }
}

// ========================================
// 设备控制
// ========================================
void RealTimeDate::on_pbtAir_clicked() {
    if (!m_deviceController->isLocked()) {
        QMessageBox::warning(this, "操作受限", 
            "当前处于自动模式，无法手动控制！\n\n"
            "如需手动控制风扇，请：\n"
            "1. 点击\"自动模式\"按钮退出自动模式\n"
            "2. 手动控制按钮将变为可用状态");
        return;
    }
    
    if (!m_serialManager->isConnected()) {
        QMessageBox::warning(this, "无法控制", 
            "串口未连接，无法发送控制命令！\n\n"
            "请先连接串口。");
        return;
    }
    
    m_deviceController->toggleFan();
    
    QString statusMsg = m_deviceController->isFanOn() 
        ? "风扇已启动！\n\n风扇将开始工作进行通风。" 
        : "风扇已停止！\n\n风扇已停止运行。";
    QMessageBox::information(this, "控制成功", statusMsg);
}

void RealTimeDate::on_pbtLight_clicked() {
    if (!m_deviceController->isLocked()) {
        QMessageBox::warning(this, "操作受限", 
            "当前处于自动模式，无法手动控制！\n\n"
            "如需手动控制补光灯，请：\n"
            "1. 点击\"自动模式\"按钮退出自动模式\n"
            "2. 手动控制按钮将变为可用状态");
        return;
    }
    
    if (!m_serialManager->isConnected()) {
        QMessageBox::warning(this, "无法控制", 
            "串口未连接，无法发送控制命令！\n\n"
            "请先连接串口。");
        return;
    }
    
    m_deviceController->toggleLight();
    
    QString statusMsg = m_deviceController->isLightOn() 
        ? "补光灯已开启！\n\n正在为植物提供补充光照。" 
        : "补光灯已关闭！\n\n补光灯已停止工作。";
    QMessageBox::information(this, "控制成功", statusMsg);
}

void RealTimeDate::on_pbtWater_clicked() {
    if (!m_deviceController->isLocked()) {
        QMessageBox::warning(this, "操作受限", 
            "当前处于自动模式，无法手动控制！\n\n"
            "如需手动控制水泵，请：\n"
            "1. 点击\"自动模式\"按钮退出自动模式\n"
            "2. 手动控制按钮将变为可用状态");
        return;
    }
    
    if (!m_serialManager->isConnected()) {
        QMessageBox::warning(this, "无法控制", 
            "串口未连接，无法发送控制命令！\n\n"
            "请先连接串口。");
        return;
    }
    
    m_deviceController->togglePump();
    
    QString statusMsg = m_deviceController->isPumpOn() 
        ? "水泵已启动！\n\n正在进行灌溉，请注意水量。\n\n⚠️ 提示：请勿长时间浇灌，避免积水。" 
        : "水泵已停止！\n\n灌溉已完成。";
    QMessageBox::information(this, "控制成功", statusMsg);
}

void RealTimeDate::on_Automatic_clicked() {
    if (!m_serialManager->isConnected()) {
        QMessageBox::warning(this, "无法切换模式", 
            "串口未连接，无法发送模式切换命令！\n\n"
            "请先连接串口。");
        return;
    }
    
    bool currentlyManual = m_deviceController->isLocked();
    
    if (currentlyManual) {
        // 切换到自动模式
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "切换到自动模式", 
            "确定要切换到自动模式吗？\n\n"
            "自动模式下：\n"
            "• 系统将根据设定的阈值自动控制设备\n"
            "• 手动控制按钮将被禁用\n"
            "• 执行器由Arduino自动管理\n\n"
            "建议：切换前请确保已设置好阈值参数。",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::No) return;
        
        m_deviceController->setAutoMode(true);
        
        QMessageBox::information(this, "模式切换成功", 
            "已切换到自动模式！\n\n"
            "系统将根据以下阈值自动控制：\n"
            "• 温度阈值 → 控制风扇\n"
            "• 湿度阈值 → 控制水泵\n"
            "• 光照阈值 → 控制补光灯\n\n"
            "手动控制已禁用。");
        
    } else {
        // 切换到手动模式
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "切换到手动模式", 
            "确定要切换到手动模式吗？\n\n"
            "手动模式下：\n"
            "• 您需要手动控制所有执行器\n"
            "• 自动控制功能将停止\n"
            "• 可能需要频繁关注环境参数\n\n"
            "建议：仅在需要精确控制或测试时使用。",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::No) return;
        
        m_deviceController->setAutoMode(false);
        
        QMessageBox::information(this, "模式切换成功", 
            "已切换到手动模式！\n\n"
            "现在可以使用左侧的手动控制按钮：\n"
            "• 手动通风\n"
            "• 手动补光\n"
            "• 手动灌溉");
    }
}

// ========================================
// 数据接收回调
// ========================================
void RealTimeDate::onSensorDataReceived(const SensorRecord& data) {
    if (!m_isCollecting) {
        return;
    }
    
    // 添加到图表 - 就这么简单！
    m_chartManager->addDataPoint(data);
    
    qDebug() << "接收传感器数据: Temp=" << data.air_temp 
             << "AirHum=" << data.air_humid 
             << "SoilHum=" << data.soil_humid
             << "Light=" << data.light_intensity;
}

void RealTimeDate::onActuatorStateReceived(const ActuatorStateData& data) {
    // 更新设备状态
    m_deviceController->updateDeviceState(data);
    
    qDebug() << "接收设备状态: Fan=" << data.fanStatus 
             << "Lamp=" << data.lampStatus 
             << "Pump=" << data.pumpStatus
             << "Auto=" << data.autoMode;
}

void RealTimeDate::onTimeWeatherReceived(const TimeWeatherData& data) {
    QString timeStr = QString("%1:%2")
        .arg(data.hour, 2, 10, QChar('0'))
        .arg(data.minute, 2, 10, QChar('0'));
    
    qDebug() << "接收时间天气:" << timeStr 
             << "温度=" << data.tempNow 
             << "天气=" << data.weatherCode;
}

// ========================================
// 设备状态变化回调
// ========================================
void RealTimeDate::onFanStateChanged(bool isOn) {
    ui->rbtAir_open->setChecked(isOn);
    ui->rbtAir_close->setChecked(!isOn);
    qDebug() << "风扇状态更新:" << (isOn ? "开启" : "关闭");
}

void RealTimeDate::onLightStateChanged(bool isOn) {
    ui->rbtLight_open->setChecked(isOn);
    ui->rbtLight_close->setChecked(!isOn);
    qDebug() << "灯光状态更新:" << (isOn ? "开启" : "关闭");
}

void RealTimeDate::onPumpStateChanged(bool isOn) {
    ui->rbtWater_open->setChecked(isOn);
    ui->rbtWater_close->setChecked(!isOn);
    qDebug() << "水泵状态更新:" << (isOn ? "开启" : "关闭");
}

void RealTimeDate::onAutoModeChanged(bool isAuto) {
    updateDeviceButtonsUI();
    qDebug() << "模式更新:" << (isAuto ? "自动" : "手动");
}

// ========================================
// UI更新辅助函数
// ========================================
void RealTimeDate::updateDeviceButtonsUI() {
    bool isManual = m_deviceController->isLocked();
    
    // 设置按钮可用状态
    ui->pbtAir->setEnabled(isManual);
    ui->pbtLight->setEnabled(isManual);
    ui->pbtWater->setEnabled(isManual);
    
    // 设置按钮样式
    QString style = isManual ? "" : "QPushButton { color: #ffffff; background-color: #000000; }";
    ui->pbtAir->setStyleSheet(style);
    ui->pbtLight->setStyleSheet(style);
    ui->pbtWater->setStyleSheet(style);
    
    // 更新单选按钮状态
    ui->rbtAir_close->setText(isManual ? "工作结束" : "已自动");
    ui->rbtLight_close->setText(isManual ? "灯光关闭" : "已自动");
    ui->rbtWater_close->setText(isManual ? "浇灌完成" : "已自动");
    
    // 禁用单选按钮直接点击
    ui->rbtAir_open->setEnabled(!isManual);
    ui->rbtAir_close->setEnabled(!isManual);
    ui->rbtLight_open->setEnabled(!isManual);
    ui->rbtLight_close->setEnabled(!isManual);
    ui->rbtWater_open->setEnabled(!isManual);
    ui->rbtWater_close->setEnabled(!isManual);
}

// ========================================
// 阈值设置 - 温度
// ========================================
void RealTimeDate::on_hsr_High_Temperature_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Temperature->value();
    ui->let_High_Temperature->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    // 更新阈值管理器
    int low = ui->hsr_Low_Temperature->value();
    m_thresholdManager->setTemperatureThreshold(value, low);
    
    qDebug() << "温度高阈值设置为:" << value;
}

void RealTimeDate::on_let_High_Temperature_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_High_Temperature->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Temperature_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Temperature->value();
    ui->let_Low_Temperature->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    // 更新阈值管理器
    int high = ui->hsr_High_Temperature->value();
    m_thresholdManager->setTemperatureThreshold(high, value);
    
    qDebug() << "温度低阈值设置为:" << value;
}

void RealTimeDate::on_let_Low_Temperature_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Temperature->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// ========================================
// 阈值设置 - 空气湿度
// ========================================
void RealTimeDate::on_hsr_High_Air_Humidity_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Air_Humidity->value();
    ui->let_High_Air_Humidity->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    int low = ui->hsr_Low_Air_Humidity->value();
    m_thresholdManager->setAirHumidityThreshold(value, low);
    
    qDebug() << "空气湿度高阈值设置为:" << value;
}

void RealTimeDate::on_let_High_Air_Humidity_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_High_Air_Humidity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Air_Humidity_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Air_Humidity->value();
    ui->let_Low_Air_Humidity->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    int high = ui->hsr_High_Air_Humidity->value();
    m_thresholdManager->setAirHumidityThreshold(high, value);
    
    qDebug() << "空气湿度低阈值设置为:" << value;
}

void RealTimeDate::on_let_Low_Air_Humidity_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Air_Humidity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// ========================================
// 阈值设置 - 光照强度
// ========================================
void RealTimeDate::on_hsr_High_Light_Intensity_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Light_Intensity->value();
    ui->let_High_Light_Intensity->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    int low = ui->hsr_Low_Light_Intensity->value();
    m_thresholdManager->setLightIntensityThreshold(value, low);
    
    qDebug() << "光照强度高阈值设置为:" << value;
}

void RealTimeDate::on_let_High_Light_Intensity_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_High_Light_Intensity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Light_Intensity_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Light_Intensity->value();
    ui->let_Low_Light_Intensity->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    int high = ui->hsr_High_Light_Intensity->value();
    m_thresholdManager->setLightIntensityThreshold(high, value);
    
    qDebug() << "光照强度低阈值设置为:" << value;
}

void RealTimeDate::on_let_Low_Light_Intensity_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Light_Intensity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// ========================================
// 阈值设置 - 土壤湿度
// ========================================
void RealTimeDate::on_hsr_High_Soil_Moisture_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Soil_Moisture->value();
    ui->let_High_Soil_Moisture->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    int low = ui->hsr_Low_Soil_Moisture->value();
    m_thresholdManager->setSoilMoistureThreshold(value, low);
    
    qDebug() << "土壤湿度高阈值设置为:" << value;
}

void RealTimeDate::on_let_High_Soil_Moisture_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_High_Soil_Moisture->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Soil_Moisture_sliderReleased() {
    if (m_isUpdatingSlider) return;
    
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Soil_Moisture->value();
    ui->let_Low_Soil_Moisture->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
    
    int high = ui->hsr_High_Soil_Moisture->value();
    m_thresholdManager->setSoilMoistureThreshold(high, value);
    
    qDebug() << "土壤湿度低阈值设置为:" << value;
}

void RealTimeDate::on_let_Low_Soil_Moisture_textChanged(const QString &arg1) {
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok) {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Soil_Moisture->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// ========================================
// 辅助函数 - 未使用（保留接口兼容性）
// ========================================
void RealTimeDate::updateSensorData() {
    // 空实现 - 由串口数据接收驱动
}

void RealTimeDate::updateThresholdUI() {
    // 预留接口
}

