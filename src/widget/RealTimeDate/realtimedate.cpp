//
// Created by Trubson on 2025/12/3.
// MVVM Architecture Version
//

#include "realtimedate.h"

#include <mmintrin.h>

#include "ui_RealTimeDate.h"

#include <QVBoxLayout>
#include <QWheelEvent>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QFrame>

#include "MyToast.h"
#include "model/Database/Database.h"

QT_CHARTS_USE_NAMESPACE

// ========================================
// 自定义 ChartView - 支持鼠标滚轮缩放
// ========================================
class CustomChartView : public QChartView
{
public:
    CustomChartView(QChart* chart, QWidget* parent = nullptr)
        : QChartView(chart, parent)
    {
        setRubberBand(QChartView::RectangleRubberBand);
        setRenderHint(QPainter::Antialiasing);
    }

protected:
    void wheelEvent(QWheelEvent* event) override
    {
        QChart* chart = this->chart();
        int delta = event->angleDelta().y();
        qreal factor = delta > 0 ? 1.1 : 0.9;
        chart->zoom(factor);
        QChartView::wheelEvent(event);
    }
};

// ========================================
// 构造函数 - MVVM 架构初始化
// ========================================
RealTimeDate::RealTimeDate(QWidget* parent)
    : QWidget(parent)
      , ui(new Ui::RealTimeDate)
      , m_chart(nullptr)
      , m_chartView(nullptr)
      , m_temperatureSeries(nullptr)
      , m_airHumiditySeries(nullptr)
      , m_soilHumiditySeries(nullptr)
      , m_lightIntensitySeries(nullptr)
      , m_axisX(nullptr)
      , m_axisY(nullptr)
      , m_serialViewModel(nullptr)
      , m_sensorViewModel(nullptr)
      , m_controlViewModel(nullptr)
      , m_chartViewModel(nullptr)
      , m_settingViewModel(nullptr)
      , m_serialPort(nullptr)
      , m_isCollecting(false)
      , m_isUpdatingSlider(false)
      , m_isUpdatingLineEdit(false)
{
    ui->setupUi(this);
    
    // 设置布局策略，支持窗口大小调整
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 移除所有子组件的固定尺寸限制，使其能够自适应
    removeSizeConstraints();
    
    qDebug() << "========================================";
    qDebug() << "🚀 RealTimeDate (MVVM 架构) 开始初始化";
    qDebug() << "========================================";

    // 加载样式表
    loadStyleSheet();

    // 步骤1：创建 ViewModel 实例
    setupViewModels();

    // 步骤2：初始化图表
    initializeChart();

    // 步骤3：连接 ViewModel 信号槽
    connectViewModelSignals();

    // 步骤4：初始化 UI
    initializeUI();

    qDebug() << "========================================";
    qDebug() << "✅ RealTimeDate (MVVM 架构) 初始化完成";
    qDebug() << "========================================";
}

RealTimeDate::~RealTimeDate()
{
    qDebug() << "🔚 RealTimeDate 析构";
    delete ui;
}

// ========================================
// 步骤1：创建 ViewModel 实例
// ========================================
void RealTimeDate::setupViewModels()
{
    qDebug() << "创建 ViewModel 实例...";

    // 1. 设置 ViewModel（最先创建，提供配置）
    m_settingViewModel = new SettingViewModel(this);
    m_settingViewModel->loadSettings();
    qDebug() << "  SettingViewModel 创建完成";

    // 2. 传感器 ViewModel
    m_sensorViewModel = new SensorViewModel(this);
    qDebug() << "  SensorViewModel 创建完成";

    // 3. 控制 ViewModel
    m_controlViewModel = new ControlViewModel(this);
    qDebug() << "  ControlViewModel 创建完成";

    // 4. 图表 ViewModel（使用设置中的最大点数）
    m_chartViewModel = new ChartViewModel(this);
    m_chartViewModel->setMaxDataCount(m_settingViewModel->getChartMaxPoints());
    qDebug() << "  ChartViewModel 创建完成，最大点数="
        << m_settingViewModel->getChartMaxPoints();

    // 5. 串口 ViewModel（需要 QSerialPort 实例）
    m_serialPort = new QSerialPort(this);
    m_serialViewModel = new SerialViewModel(m_serialPort, this);
    qDebug() << "  SerialViewModel 创建完成";
    
    // 6. WebSocket ViewModel
    m_webSocketViewModel = new WebSocketViewModel(this);
    qDebug() << "  WebSocketViewModel 创建完成";
}

// ========================================
// 步骤2：初始化图表
// ========================================
void RealTimeDate::initializeChart()
{
    qDebug() << "步骤2：初始化图表...";

    // 创建图表
    m_chart = new QChart();
    m_chart->setTitle("大棚环境数据实时监控");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->setTheme(QChart::ChartThemeLight);

    // 创建数据序列
    m_temperatureSeries = new QLineSeries();
    m_temperatureSeries->setName("温度 (°C)");
    m_temperatureSeries->setColor(QColor(255, 87, 51)); // 红色

    m_airHumiditySeries = new QLineSeries();
    m_airHumiditySeries->setName("空气湿度 (%)");
    m_airHumiditySeries->setColor(QColor(51, 153, 255)); // 蓝色

    m_soilHumiditySeries = new QLineSeries();
    m_soilHumiditySeries->setName("土壤湿度 (%)");
    m_soilHumiditySeries->setColor(QColor(139, 69, 19)); // 棕色

    m_lightIntensitySeries = new QLineSeries();
    m_lightIntensitySeries->setName("光照强度 (%)");
    m_lightIntensitySeries->setColor(QColor(255, 215, 0)); // 金色

    // 添加序列到图表
    m_chart->addSeries(m_temperatureSeries);
    m_chart->addSeries(m_airHumiditySeries);
    m_chart->addSeries(m_soilHumiditySeries);
    m_chart->addSeries(m_lightIntensitySeries);

    // 创建坐标轴
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("hh:mm:ss");
    m_axisX->setTitleText("时间");
    m_axisX->setTickCount(10);

    m_axisY = new QValueAxis();
    m_axisY->setRange(0, 100);
    m_axisY->setTitleText("数值");
    m_axisY->setLabelFormat("%d");

    // 设置坐标轴
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // 绑定序列到坐标轴
    m_temperatureSeries->attachAxis(m_axisX);
    m_temperatureSeries->attachAxis(m_axisY);
    m_airHumiditySeries->attachAxis(m_axisX);
    m_airHumiditySeries->attachAxis(m_axisY);
    m_soilHumiditySeries->attachAxis(m_axisX);
    m_soilHumiditySeries->attachAxis(m_axisY);
    m_lightIntensitySeries->attachAxis(m_axisX);
    m_lightIntensitySeries->attachAxis(m_axisY);

    // 设置图例 位置
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    // 创建 ChartView 启用锯齿状渲染
    m_chartView = new CustomChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // 将图表添加到 frame_2
    if (ui->frame_2)
    {
        QVBoxLayout* layout = new QVBoxLayout(ui->frame_2);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(m_chartView);
        ui->frame_2->setLayout(layout);
    }

    //设置状态按钮为不可选中状态
    ui->rbtAir_open->setEnabled(false);
    ui->rbtAir_close->setEnabled(false);
    ui->rbtLight_close->setEnabled(false);
    ui->rbtLight_open->setEnabled(false);
    ui->rbtWater_open->setEnabled(false);
    ui->rbtWater_close->setEnabled(false);

    qDebug() << "  ✅ 图表初始化完成（4条曲线）";
}

// ========================================
// 步骤3：连接 ViewModel 信号槽（数据绑定）
// ========================================
void RealTimeDate::connectViewModelSignals()
{
    qDebug() << "🔗 步骤3：连接 ViewModel 信号槽...";

    // ===== SerialViewModel 信号 =====
    connect(m_serialViewModel, &SerialViewModel::sensorDataReceived,
            this, &RealTimeDate::onSensorDataReceived);
    connect(m_serialViewModel, &SerialViewModel::actuatorStateReceived,
            this, &RealTimeDate::onActuatorStateReceived);
    connect(m_serialViewModel, &SerialViewModel::timeWeatherReceived,
            this, &RealTimeDate::onTimeWeatherReceived);
    
    // ===== WebSocketViewModel 信号 =====
    connect(m_webSocketViewModel, &WebSocketViewModel::sensorDataReceived,
            this, &RealTimeDate::onSensorDataReceived);
    connect(m_webSocketViewModel, &WebSocketViewModel::actuatorStateReceived,
            this, &RealTimeDate::onActuatorStateReceived);
    connect(m_webSocketViewModel, &WebSocketViewModel::timeWeatherReceived,
            this, &RealTimeDate::onTimeWeatherReceived);
    connect(m_webSocketViewModel, &WebSocketViewModel::heartBeatReceived,
            this, &RealTimeDate::onHeartBeatReceived);
    connect(m_webSocketViewModel, &WebSocketViewModel::thresholdReceived,
            this, &RealTimeDate::onThresholdReceived);
    connect(m_webSocketViewModel, &WebSocketViewModel::connected,
            this, [this]() {
                MyToast::success(this, "连接成功", "WebSocket连接成功！");
                if (ui->btnWebsocketLink) {
                    ui->btnWebsocketLink->setText("断开WebSocket");
                }
                // WebSocket连接成功后，自动开始数据采集
                if (!m_isCollecting) {
                    m_isCollecting = true;
                    sendDataCollectControlCommand(true);
                    qDebug() << "✅ WebSocket连接成功，自动开始数据采集";
                }
            });
    connect(m_webSocketViewModel, &WebSocketViewModel::disconnected,
            this, [this]() {
                MyToast::info(this, "已断开", "WebSocket已断开连接");
                if (ui->btnWebsocketLink) {
                    ui->btnWebsocketLink->setText("连接WebSocket");
                }
            });
    connect(m_webSocketViewModel, &WebSocketViewModel::errorOccurred,
            this, [this](const QString& error) {
                MyToast::error(this, "连接错误", error);
            });
    connect(m_serialViewModel, &SerialViewModel::heartBeatReceived,
            this, &RealTimeDate::onHeartBeatReceived);
    //connect(m_serialViewModel,&SerialViewModel::thresholdReceived,
    //        this,&RealTimeDate::onThresholdReceived);
    qDebug() << "  ✅ SerialViewModel 信号连接完成";

    // ===== ControlViewModel 信号 =====
    connect(m_controlViewModel, &ControlViewModel::fanStateChanged,
            this, [this](bool isOn)
            {
                ui->rbtAir_open->setChecked(isOn);
                ui->rbtAir_close->setChecked(!isOn);
                qDebug() << "🌀 UI更新：风扇=" << (isOn ? "开" : "关");
            });

    connect(m_controlViewModel, &ControlViewModel::pumpStateChanged,
            this, [this](bool isOn)
            {
                ui->rbtWater_open->setChecked(isOn);
                ui->rbtWater_close->setChecked(!isOn);
                qDebug() << "💧 UI更新：水泵=" << (isOn ? "开" : "关");
            });

    connect(m_controlViewModel, &ControlViewModel::lampStateChanged,
            this, [this](bool isOn)
            {
                ui->rbtLight_open->setChecked(isOn);
                ui->rbtLight_close->setChecked(!isOn);
                qDebug() << "💡 UI更新：灯光=" << (isOn ? "开" : "关");
            });

    connect(m_controlViewModel, &ControlViewModel::autoModeChanged,
            this, [this](bool isAuto)
            {
                updateDeviceButtonsUI();
                qDebug() << "UI更新：模式=" << (isAuto ? "自动" : "手动");
            });

    qDebug() << " ControlViewModel 信号连接完成";

    // ===== ChartViewModel 信号 =====
    connect(m_chartViewModel, &ChartViewModel::dataAdded,
            this, [this](const SensorRecord& data)
            {
                qDebug() << "图表数据已添加，总数=" << m_chartViewModel->getDataCount();
            });

    connect(m_chartViewModel, &ChartViewModel::dataCleared,
            this, [this]()
            {
                qDebug() << "图表数据已清空";
            });

    qDebug() << "  ChartViewModel 信号连接完成";

    // ===== SettingViewModel 信号 =====
    connect(m_settingViewModel, &SettingViewModel::thresholdChanged,
            this, &RealTimeDate::updateThresholdUI);
    qDebug() << "  SettingViewModel 信号连接完成";
}

// ========================================
// 步骤4：初始化 UI
// ========================================
void RealTimeDate::initializeUI()
{
    qDebug() << "步骤4：初始化 UI...";

    // 初始化串口列表
    const auto& serialPorts = QSerialPortInfo::availablePorts();
    ui->cbxSerial->clear();
    for (const auto& port : serialPorts)
    {
        ui->cbxSerial->addItem(port.portName());
    }

    // 从 SettingViewModel 加载上次使用的串口
    QString lastPort = m_settingViewModel->getLastSerialPort();
    if (!lastPort.isEmpty())
    {
        int index = ui->cbxSerial->findText(lastPort);
        if (index >= 0)
        {
            ui->cbxSerial->setCurrentIndex(index);
        }
    }

    // 初始化阈值 UI
    updateThresholdUI();

    // 初始化设备状态显示
    updateDeviceButtonsUI();
    
    // 初始化连接模式按钮状态
    if (ui->btnModechange)
    {
        ui->btnModechange->setText("切换到WebSocket模式");
        m_currentMode = MODE_SERIAL;  // 默认串口模式
    }
    
    // 初始化WebSocket连接按钮状态
    if (ui->btnWebsocketLink)
    {
        ui->btnWebsocketLink->setText("连接WebSocket");
    }

    qDebug() << "  ✅ UI 初始化完成";
}

// ========================================
// 串口连接（带互斥检查）
// ========================================
void RealTimeDate::on_pbtlink_clicked()
{
    // 检查WebSocket是否已连接
    if (m_webSocketViewModel->isConnected())
    {
        QMessageBox::warning(this, "连接冲突",
                             "WebSocket已连接，请先断开WebSocket连接！\n"
                             "串口和WebSocket不能同时工作。");
        return;
    }
    
    if (!m_serialPort->isOpen())
    {
        // ========== 连接串口 ==========
        QString portName = ui->cbxSerial->currentText();

        if (portName.isEmpty())
        {
            QMessageBox::warning(this, "串口选择错误",
                                 "请先在下拉框中选择一个串口！");
            return;
        }

        // 配置串口
        m_serialPort->setPortName(portName);
        m_serialPort->setBaudRate(m_settingViewModel->getSerialBaudRate());
        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setParity(QSerialPort::NoParity);
        m_serialPort->setStopBits(QSerialPort::OneStop);

        if (m_serialPort->open(QIODevice::ReadWrite))
        {
            // 连接成功
            m_currentMode = MODE_SERIAL;
            m_serialViewModel->startListening();
            ui->pbtlink->setText("断开");

            // 保存串口到设置
            m_settingViewModel->setLastSerialPort(portName);
            MyToast::success(this, "连接成功",QString("串口 %1 连接成功！\n波特率: %2")
                             .arg(portName)
                             .arg(m_settingViewModel->getSerialBaudRate()));

            qDebug() << "串口连接成功:" << portName;
        }
        else
        {
            QMessageBox::critical(this, "串口连接失败",
                                  QString("无法打开串口 %1\n错误: %2")
                                  .arg(portName)
                                  .arg(m_serialPort->errorString()));

            qWarning() << "串口连接失败:" << m_serialPort->errorString();
        }
    }
    else
    {
        // ========== 断开串口 ==========
        auto reply = QMessageBox::question(this, "确认断开",
                                           "确定要断开串口连接吗？\n断开后将停止数据采集。",
                                           QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            m_serialViewModel->stopListening();
            m_serialPort->close();
            m_isCollecting = false;
            m_currentMode = MODE_SERIAL;  // 重置模式
            ui->pbtlink->setText("连接");
            MyToast::info(this, "已断开","串口已断开连接…");
            qDebug() << "串口已断开";
        }
    }
}

// ========================================
// 数据采集控制
// ========================================
void RealTimeDate::on_pbtStart_clicked()
{
    if (!isAnyConnectionActive())
    {
        MyToast::error(this, "无法开始采集", "请先连接串口或WebSocket！");
        return;
    }

    if (!m_isCollecting)
    {
        m_isCollecting = true;

        // 发送数据采集启动命令
        sendDataCollectControlCommand(true);

        MyToast::info(this, "采集已开始",
                                 "数据采集已开始！\n数据更新间隔：约10秒");

        qDebug() << "数据采集已开始";
    }
    else
    {
        MyToast::info(this, "提示", "数据采集已经在运行中！");
    }
}

void RealTimeDate::on_pbtEnd_clicked()
{
    if (m_isCollecting)
    {
        auto reply = QMessageBox::question(this, "确认停止",
                                           "确定要停止数据采集吗？",
                                           QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            m_isCollecting = false;

            // 发送数据采集停止命令
            sendDataCollectControlCommand(false);

            QMessageBox::information(this, "已停止", "数据采集已停止。");
            qDebug() << "数据采集已停止";
        }
    }
    else
    {
        QMessageBox::information(this, "提示", "当前没有正在进行的数据采集。");
    }
}

void RealTimeDate::on_pbtClaer_clicked()
{
    int dataCount = m_chartViewModel->getDataCount();

    if (dataCount == 0)
    {
        QMessageBox::information(this, "提示", "当前没有数据需要清除。");
        return;
    }

    auto reply = QMessageBox::warning(this, "确认清除",
                                      QString("确定要清除所有数据吗？\n将清除 %1 个数据点").arg(dataCount),
                                      QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // 使用 ChartViewModel 清空数据
        m_chartViewModel->clearAllData();

        // 清空图表显示
        if (m_chart)
        {
            m_chart->removeAllSeries();
        }

        QMessageBox::information(this, "清除成功", "所有数据已清除！");
        qDebug() << "数据已清除";
    }
}

void RealTimeDate::on_RefreshClicked()
{
    sendGetDataCommand(true);
}

// ========================================
// 设备控制
// ========================================
void RealTimeDate::on_pbtAir_clicked()
{
    if (!isAnyConnectionActive())
    {
        QMessageBox::warning(this, "无法控制", "请先连接串口或WebSocket！");
        return;
    }

    if (m_controlViewModel->isAutoMode())
    {
        QMessageBox::warning(this, "操作受限", "当前处于自动模式，无法手动控制！");
        return;
    }

    // 使用 ControlViewModel 切换状态
    bool newState = m_controlViewModel->toggleFan();

    // 根据当前模式发送控制命令
    sendMotorControlCommand(
        newState ? 1 : 0,
        80, // 默认速度
        m_controlViewModel->isPumpOn() ? 1 : 0,
        m_controlViewModel->isLampOn() ? 1 : 0
    );

    MyToast::success(this, "控制成功",
                             QString("风扇已%1！").arg(newState ? "开启" : "关闭"));
}

void RealTimeDate::on_pbtLight_clicked()
{
    if (!isAnyConnectionActive())
    {
        MyToast::warning(this, "无法控制", "请先连接串口或WebSocket！");
        return;
    }

    if (m_controlViewModel->isAutoMode())
    {
        MyToast::warning(this, "操作受限", "当前处于自动模式！");
        return;
    }

    bool newState = m_controlViewModel->toggleLamp();

    sendMotorControlCommand(
        m_controlViewModel->isFanOn() ? 1 : 0,
        80,
        m_controlViewModel->isPumpOn() ? 1 : 0,
        newState ? 1 : 0
    );

    MyToast::info(this, "控制成功",
                             QString("补光灯已%1！").arg(newState ? "开启" : "关闭"));
}

void RealTimeDate::on_pbtWater_clicked()
{
    if (!isAnyConnectionActive())
    {
        MyToast::warning(this, "无法控制", "请先连接串口或WebSocket！");
        return;
    }

    if (m_controlViewModel->isAutoMode())
    {
        MyToast::warning(this, "操作受限", "当前处于自动模式！");
        return;
    }

    bool newState = m_controlViewModel->togglePump();

    sendMotorControlCommand(
        m_controlViewModel->isFanOn() ? 1 : 0,
        80,
        newState ? 1 : 0,
        m_controlViewModel->isLampOn() ? 1 : 0
    );

    MyToast::info(this, "控制成功",
                             QString("水泵已%1！").arg(newState ? "开启" : "关闭"));
}

void RealTimeDate::on_Automatic_clicked()
{
    if (!isAnyConnectionActive())
    {
        MyToast::warning(this, "无法切换模式", "请先连接串口或WebSocket！");
        return;
    }

    bool currentlyManual = !m_controlViewModel->isAutoMode();

    QString message = currentlyManual
                          ? "确定要切换到自动模式吗？\n系统将根据阈值自动控制设备。"
                          : "确定要切换到手动模式吗？\n您需要手动控制所有设备。";

    auto reply = QMessageBox::question(this, "模式切换", message,
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        bool newMode = m_controlViewModel->toggleAutoMode();
        sendAutoModeControlCommand(newMode);

        MyToast::success(this, "模式切换成功",
                                 QString("已切换到%1模式！").arg(newMode ? "自动" : "手动"));
    }
}

// ========================================
// ViewModel 数据接收回调
// ========================================
void RealTimeDate::onSensorDataReceived(const SensorRecord& data)
{
    emit sensorDataReceived(data);
    
    // 如果未开始采集，自动开始（适用于WebSocket模式）
    if (!m_isCollecting && isAnyConnectionActive())
    {
        qDebug() << "📥 接收到数据但采集未开始，自动开始采集";
        m_isCollecting = true;
    }
    
    if (!m_isCollecting)
    {
        qDebug() << "⚠️ 数据采集未开始，忽略数据";
        return;
    }

    qDebug() << "📥 接收传感器数据";

    // 1. 使用 SensorViewModel 验证数据
    if (!SensorViewModel::validateSensorData(data))
    {
        qWarning() << "⚠️ 数据验证失败";
        return;
    }

    // 2. 更新 UI 标签（使用 SensorViewModel 的格式化函数）
    updateSensorLabels(data);

    // 3. 使用 ChartViewModel 添加数据
    m_chartViewModel->addData(data);

    // 4. 更新图表显示
    updateChartDisplay(data);

    // 5. 保存到数据库（如果启用）
    if (m_settingViewModel->getAutoSaveToDatabase())
    {
        Database::instance().insert(data);
        qDebug() << "  💾 数据已保存到数据库";
    }
}

void RealTimeDate::onActuatorStateReceived(const ActuatorStateData& data)
{
    qDebug() << "📥 接收执行器状态";

    // 使用 ControlViewModel 更新状态
    // ViewModel 会自动发出信号更新 UI
    m_controlViewModel->updateState(data);
}

void RealTimeDate::onTimeWeatherReceived(const TimeWeatherData& data)
{
    qDebug() << "📥 接收时间天气";

    QString timeStr = QString("%1:%2")
                      .arg(data.hour, 2, 10, QChar('0'))
                      .arg(data.minute, 2, 10, QChar('0'));

    // 更新UI
    // ui->lblTime->setText(timeStr);
    // ui->lblWeather->setText(QString("%1°C").arg(data.tempNow));
}

void RealTimeDate::onHeartBeatReceived()
{
    qDebug() << "💓 接收心跳包";
    // 更新连接状态指示
}

void RealTimeDate::onThresholdReceived(const Threshold& threshold)
{
    m_settingViewModel->setFanOffThreshold(threshold.fanOffThreshold);
    m_settingViewModel->setFanOnThreshold(threshold.fanOnThreshold);
    m_settingViewModel->setLampOffThreshold(threshold.lampOffThreshold);
    m_settingViewModel->setLampOnThreshold(threshold.lampONThreshold);
    m_settingViewModel->setPumpOffThreshold(threshold.DumpOffThreshold);
    m_settingViewModel->setPumpOnThreshold(threshold.DumpOnThreshold);
    updateThresholdUI();
}

// ========================================
// UI 更新辅助函数
// ========================================
void RealTimeDate::updateSensorLabels(const SensorRecord& data)
{
    // 使用 SensorViewModel 的格式化函数
    QString tempStr = SensorViewModel::formatTemperature(data.air_temp);
    QString airHumStr = SensorViewModel::formatHumidity(data.air_humid);
    QString soilHumStr = SensorViewModel::formatHumidity(data.soil_humid);
    QString lightStr = SensorViewModel::formatLightIntensity(data.light_intensity);

    // 更新UI标签（需要根据实际UI组件名称调整）
    // ui->lblTemperature->setText(tempStr);
    // ui->lblAirHumidity->setText(airHumStr);
    // ui->lblSoilHumidity->setText(soilHumStr);
    // ui->lblLightIntensity->setText(lightStr);

    // 显示数据等级
    // ui->lblTempLevel->setText(SensorViewModel::getTemperatureLevel(data.air_temp));
}

void RealTimeDate::updateChartDisplay(const SensorRecord& data)
{
    // 从 ChartViewModel 获取所有数据
    auto allData = m_chartViewModel->getAllData();

    if (allData.isEmpty())
    {
        qDebug() << "📊 图表数据为空，跳过更新";
        return;
    }

    // 清空现有数据点
    m_temperatureSeries->clear();
    m_airHumiditySeries->clear();
    m_soilHumiditySeries->clear();
    m_lightIntensitySeries->clear();

    // 添加所有数据点
    QDateTime minTime, maxTime;
    double maxValue = 0;

    for (int i = 0; i < allData.size(); ++i)
    {
        const auto& record = allData[i];

        // 解析时间
        QDateTime dateTime = QDateTime::fromString(
            QString::fromStdString(record.record_time),
            "yyyy-MM-dd hh:mm:ss"
        );

        if (!dateTime.isValid())
        {
            dateTime = QDateTime::currentDateTime().addSecs(-allData.size() + i);
        }

        qint64 timestamp = dateTime.toMSecsSinceEpoch();

        // 更新时间范围
        if (i == 0 || dateTime < minTime) minTime = dateTime;
        if (i == 0 || dateTime > maxTime) maxTime = dateTime;

        // 添加数据点
        m_temperatureSeries->append(timestamp, record.air_temp);
        m_airHumiditySeries->append(timestamp, record.air_humid);
        m_soilHumiditySeries->append(timestamp, record.soil_humid);

        m_lightIntensitySeries->append(timestamp, record.light_intensity);

        // 更新最大值（用于Y轴范围）
        maxValue = qMax(maxValue, (double)record.air_temp);
        maxValue = qMax(maxValue, (double)record.air_humid);
        maxValue = qMax(maxValue, (double)record.soil_humid);
        maxValue = qMax(maxValue, (double)record.light_intensity);
    }

    // 更新X轴范围（显示最近的数据）
    if (minTime.isValid() && maxTime.isValid())
    {
        m_axisX->setRange(minTime, maxTime);
    }

    // 更新Y轴范围（自适应）
    if (maxValue > 0)
    {
        m_axisY->setRange(-20, qMax(100.0, maxValue * 1.2)); // 留20%余量
    }

    qDebug() << "📊 图表已更新：" << allData.size() << "个数据点"
        << "时间范围:" << minTime.toString("hh:mm:ss")
        << "-" << maxTime.toString("hh:mm:ss")
        << "Y轴范围:" << 0 << "-" << maxValue * 1.2;
}

void RealTimeDate::updateDeviceButtonsUI()
{
    bool isManual = !m_controlViewModel->isAutoMode();

    // 设置按钮可用状态
    ui->pbtAir->setEnabled(isManual);
    ui->pbtLight->setEnabled(isManual);
    ui->pbtWater->setEnabled(isManual);
}

void RealTimeDate::updateThresholdUI()
{
    // 从 SettingViewModel 读取阈值并更新UI
    ui->let_High_Temperature->setText(QString::number(m_settingViewModel->getFanOnThreshold()));
    ui->let_Low_Temperature->setText(QString::number(m_settingViewModel->getFanOffThreshold()));
    ui->hsr_High_Temperature->setValue(m_settingViewModel->getFanOnThreshold());
    ui->hsr_Low_Temperature->setValue(m_settingViewModel->getFanOffThreshold());

    ui->let_High_Soil_Moisture->setText(QString::number(m_settingViewModel->getPumpOffThreshold()));
    ui->let_Low_Soil_Moisture->setText(QString::number(m_settingViewModel->getPumpOnThreshold()));
    ui->hsr_Low_Soil_Moisture->setValue(m_settingViewModel->getPumpOnThreshold());
    ui->hsr_High_Soil_Moisture->setValue(m_settingViewModel->getPumpOffThreshold());

    ui->let_High_Light_Intensity->setText(QString::number(m_settingViewModel->getLampOffThreshold()));
    ui->let_Low_Light_Intensity->setText(QString::number(m_settingViewModel->getLampOnThreshold()));
    ui->hsr_Low_Light_Intensity->setValue(m_settingViewModel->getLampOnThreshold());
    ui->hsr_High_Light_Intensity->setValue(m_settingViewModel->getLampOffThreshold());

    qDebug() << "🎨 阈值 UI 已更新";
}

void RealTimeDate::removeSizeConstraints()
{
    // 移除主窗口的最小尺寸限制（UI文件中设置的 1692x822 太大）
    // 设置合理的最小尺寸：800x600（可以正常显示所有内容）
    setMinimumSize(800, 600);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    
    // 处理所有 Frame 组件，移除固定尺寸限制
    if (ui) {
        // 关键 Frame 组件
        QList<QFrame*> keyFrames;
        if (ui->frame) keyFrames.append(ui->frame);
        if (ui->frame_2) keyFrames.append(ui->frame_2);
        if (ui->frame_6) keyFrames.append(ui->frame_6);
        
        for (QFrame* frame : keyFrames) {
            frame->setMinimumSize(0, 0);
            frame->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }
        
        // 处理所有其他 Frame 组件
        QList<QFrame*> allFrames = findChildren<QFrame*>();
        for (QFrame* frame : allFrames) {
            if (frame && !keyFrames.contains(frame)) {
                // 移除最小尺寸限制，但保留合理的尺寸策略
                frame->setMinimumSize(0, 0);
                frame->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            }
        }
        
        // 处理 TabWidget
        QList<QTabWidget*> tabWidgets = findChildren<QTabWidget*>();
        for (QTabWidget* tab : tabWidgets) {
            tab->setMinimumSize(0, 0);
            tab->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
    }
    
    qDebug() << "✅ 已优化界面尺寸限制，最小尺寸: 800x600，支持窗口大小调整";
}

void RealTimeDate::loadStyleSheet()
{
    QFile styleFile(":/widget/RealTimeDate/realtimedate.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "✅ 样式表加载成功";
    }
    else
    {
        qDebug() << "⚠️ 样式表加载失败";
    }
}

void RealTimeDate::sendAllThresholdsToDevice()
{
    if (!isAnyConnectionActive())
    {
        qDebug() << "⚠️ 未连接（串口或WebSocket），无法发送阈值";
        return;
    }

    // 从 SettingViewModel 获取所有阈值
    uint8_t fanOn = m_settingViewModel->getFanOnThreshold();
    uint8_t fanOff = m_settingViewModel->getFanOffThreshold();
    uint8_t pumpOn = m_settingViewModel->getPumpOnThreshold();
    uint8_t pumpOff = m_settingViewModel->getPumpOffThreshold();
    uint8_t lampOn = m_settingViewModel->getLampOnThreshold();
    uint8_t lampOff = m_settingViewModel->getLampOffThreshold();

    // 根据当前模式发送阈值
    sendThresholdCommand(fanOn, fanOff, pumpOn, pumpOff, lampOn, lampOff);

    qDebug() << "📤 已发送所有阈值到下位机："
        << "风扇[" << fanOn << "," << fanOff << "]"
        << "水泵[" << pumpOn << "," << pumpOff << "]"
        << "灯光[" << lampOn << "," << lampOff << "]";
}

// ========================================
// 阈值设置槽函数（简化实现）
// ========================================
void RealTimeDate::on_hsr_High_Temperature_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Temperature->value();
    QString previous_value = ui->let_High_Temperature->text();
    if (value < ui->hsr_Low_Temperature->value()) {
        MyToast::info(this, "温度阈值错误","高阈值应大于低阈值");
        ui->hsr_High_Temperature->setValue(previous_value.toInt());
        return;
    }
    ui->let_High_Temperature->setText(QString::number(value));
    m_settingViewModel->setFanOnThreshold(value);
    m_isUpdatingLineEdit = false;

    // 发送所有阈值到下位机
    sendAllThresholdsToDevice();

    qDebug() << "🌡️ 温度高阈值已设置：" << value << "°C（温度 >" << value << " 时开风扇）";
}

void RealTimeDate::on_let_High_Temperature_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_High_Temperature->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Temperature_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Temperature->value();
    QString previous_value = ui->let_Low_Temperature->text();
    if (value > ui->hsr_High_Temperature->value()) {
        MyToast::info(this, "温度阈值错误","低阈值应小于高阈值");
        ui->hsr_Low_Temperature->setValue(previous_value.toInt());
        return;
    }
    ui->let_Low_Temperature->setText(QString::number(value));
    m_settingViewModel->setFanOffThreshold(value);
    m_isUpdatingLineEdit = false;

    // 发送所有阈值到下位机
    sendAllThresholdsToDevice();

    qDebug() << "🌡️ 温度低阈值已设置：" << value << "°C（温度 <" << value << " 时关风扇）";
}

void RealTimeDate::on_let_Low_Temperature_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Temperature->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// 空气湿度阈值
void RealTimeDate::on_hsr_High_Air_Humidity_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Air_Humidity->value();
    QString previous_value = ui->let_High_Air_Humidity->text();
    if (value < ui->hsr_Low_Air_Humidity->value()) {
        MyToast::info(this, "空气湿度阈值错误","高阈值应大于低阈值");
        ui->hsr_High_Air_Humidity->setValue(previous_value.toInt());
        return;
    }
    ui->let_High_Air_Humidity->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
}

void RealTimeDate::on_let_High_Air_Humidity_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_High_Air_Humidity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Air_Humidity_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Air_Humidity->value();
    QString previous_value = ui->let_Low_Air_Humidity->text();
    if (value > ui->hsr_High_Air_Humidity->value()) {
        MyToast::info(this, "空气湿度阈值错误","低阈值应小于高阈值");
        ui->hsr_Low_Air_Humidity->setValue(previous_value.toInt());
        return;
    }
    ui->let_Low_Air_Humidity->setText(QString::number(value));
    m_isUpdatingLineEdit = false;
}

void RealTimeDate::on_let_Low_Air_Humidity_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Air_Humidity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// 光照强度阈值
void RealTimeDate::on_hsr_High_Light_Intensity_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Light_Intensity->value();
    QString previous_value = ui->let_High_Light_Intensity->text();
    if (value < ui->hsr_Low_Light_Intensity->value()) {
        MyToast::info(this, "光照强度阈值错误","高阈值应大于低阈值");
        ui->hsr_High_Light_Intensity->setValue(previous_value.toInt());
        return;
    }
    ui->let_High_Light_Intensity->setText(QString::number(value));
    m_settingViewModel->setLampOffThreshold(value);
    m_isUpdatingLineEdit = false;

    // 发送所有阈值到下位机
    sendAllThresholdsToDevice();

    qDebug() << "💡 光照强度高阈值已设置：" << value << "（光照 >" << value << " 时关灯）";
}

void RealTimeDate::on_let_High_Light_Intensity_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_High_Light_Intensity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Light_Intensity_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Light_Intensity->value();
    QString previous_value = ui->let_Low_Light_Intensity->text();
    if (value > ui->hsr_High_Light_Intensity->value()) {
        MyToast::info(this, "光照强度阈值错误","低阈值应小于高阈值");
        ui->hsr_Low_Light_Intensity->setValue(previous_value.toInt());
        return;
    }
    ui->let_Low_Light_Intensity->setText(QString::number(value));
    m_settingViewModel->setLampOnThreshold(value);
    m_isUpdatingLineEdit = false;

    // 发送所有阈值到下位机
    sendAllThresholdsToDevice();

    qDebug() << "💡 光照强度低阈值已设置：" << value << "（光照 <" << value << " 时开灯）";
}

void RealTimeDate::on_let_Low_Light_Intensity_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Light_Intensity->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// 土壤湿度阈值
void RealTimeDate::on_hsr_High_Soil_Moisture_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_High_Soil_Moisture->value();
    QString previous_value = ui->let_High_Soil_Moisture->text();
    if (value < ui->hsr_Low_Soil_Moisture->value()) {
        MyToast::info(this, "土壤湿度阈值错误","高阈值应大于低阈值");
        ui->hsr_High_Soil_Moisture->setValue(previous_value.toInt());
        return;
    }
    ui->let_High_Soil_Moisture->setText(QString::number(value));
    m_settingViewModel->setPumpOffThreshold(value);
    m_isUpdatingLineEdit = false;

    // 发送所有阈值到下位机
    sendAllThresholdsToDevice();

    qDebug() << "💧 土壤湿度高阈值已设置：" << value << "%（湿度 >" << value << " 时关水泵）";
}

void RealTimeDate::on_let_High_Soil_Moisture_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_High_Soil_Moisture->setValue(value);
        m_isUpdatingSlider = false;
    }
}

void RealTimeDate::on_hsr_Low_Soil_Moisture_sliderReleased()
{
    if (m_isUpdatingSlider) return;
    m_isUpdatingLineEdit = true;
    int value = ui->hsr_Low_Soil_Moisture->value();
    QString previous_value = ui->let_Low_Soil_Moisture->text();
    if (value > ui->hsr_High_Soil_Moisture->value()) {
        MyToast::info(this, "土壤湿度阈值错误","低阈值应小于高阈值");
        ui->hsr_Low_Soil_Moisture->setValue(previous_value.toInt());
        return;
    }
    ui->let_Low_Soil_Moisture->setText(QString::number(value));
    m_settingViewModel->setPumpOnThreshold(value);
    m_isUpdatingLineEdit = false;

    // 发送所有阈值到下位机
    sendAllThresholdsToDevice();

    qDebug() << "💧 土壤湿度低阈值已设置：" << value << "%（湿度 <" << value << " 时开水泵）";
}

void RealTimeDate::on_let_Low_Soil_Moisture_textChanged(const QString& arg1)
{
    if (m_isUpdatingLineEdit || arg1.isEmpty()) return;
    bool ok;
    int value = arg1.toInt(&ok);
    if (ok)
    {
        m_isUpdatingSlider = true;
        ui->hsr_Low_Soil_Moisture->setValue(value);
        m_isUpdatingSlider = false;
    }
}

// ========================================
// 连接模式切换按钮
// ========================================
void RealTimeDate::on_btnModechange_clicked()
{
    // 切换连接模式
    if (m_currentMode == MODE_SERIAL)
    {
        // 切换到WebSocket模式
        if (m_serialPort->isOpen())
        {
            auto reply = QMessageBox::question(this, "切换连接模式",
                                               "当前串口已连接，切换到WebSocket模式将断开串口连接。\n"
                                               "确定要继续吗？",
                                               QMessageBox::Yes | QMessageBox::No);
            
            if (reply != QMessageBox::Yes)
            {
                return;
            }
            
            // 断开串口
            m_serialViewModel->stopListening();
            m_serialPort->close();
            ui->pbtlink->setText("连接");
            m_isCollecting = false;
        }
        
        m_currentMode = MODE_WEBSOCKET;
        ui->btnModechange->setText("切换到串口模式");
        MyToast::info(this, "模式切换", "已切换到WebSocket模式");
        qDebug() << "切换到WebSocket模式";
    }
    else
    {
        // 切换到串口模式
        if (m_webSocketViewModel->isConnected())
        {
            auto reply = QMessageBox::question(this, "切换连接模式",
                                               "当前WebSocket已连接，切换到串口模式将断开WebSocket连接。\n"
                                               "确定要继续吗？",
                                               QMessageBox::Yes | QMessageBox::No);
            
            if (reply != QMessageBox::Yes)
            {
                return;
            }
            
            // 断开WebSocket
            m_webSocketViewModel->disconnectFromServer();
            m_isCollecting = false;
        }
        
        m_currentMode = MODE_SERIAL;
        ui->btnModechange->setText("切换到WebSocket模式");
        MyToast::info(this, "模式切换", "已切换到串口模式");
        qDebug() << "切换到串口模式";
    }
}

// ========================================
// WebSocket连接按钮
// ========================================
void RealTimeDate::on_btnWebsocketLink_clicked()
{
    // 检查串口是否已连接
    if (m_serialPort->isOpen())
    {
        QMessageBox::warning(this, "连接冲突",
                             "串口已连接，请先断开串口连接！\n"
                             "串口和WebSocket不能同时工作。");
        return;
    }
    
    if (!m_webSocketViewModel->isConnected())
    {
        QString wsUrl = "ws://123.249.39.224:8080/";  // 默认地址
        m_webSocketViewModel->connectToServer(wsUrl);
        m_currentMode = MODE_WEBSOCKET;
        
        // 更新按钮文本
        ui->btnWebsocketLink->setText("断开WebSocket");
        qDebug() << "正在连接WebSocket:" << wsUrl;
        
        // 注意：m_isCollecting 会在连接成功后自动设置（见connected信号处理）
    }
    else
    {
        // 断开WebSocket
        auto reply = QMessageBox::question(this, "确认断开",
                                           "确定要断开WebSocket连接吗？\n断开后将停止数据采集。",
                                           QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes)
        {
            m_webSocketViewModel->disconnectFromServer();
            m_isCollecting = false;
            ui->btnWebsocketLink->setText("连接WebSocket");
            qDebug() << "WebSocket已断开";
        }
    }
}

// ========================================
// 切换连接模式
// ========================================
void RealTimeDate::switchConnectionMode(ConnectionMode mode)
{
    m_currentMode = mode;
    qDebug() << "切换连接模式:" << (mode == MODE_SERIAL ? "串口" : "WebSocket");
}

// ========================================
// 检查是否有任何连接处于活动状态
// ========================================
bool RealTimeDate::isAnyConnectionActive() const
{
    return m_serialPort->isOpen() || m_webSocketViewModel->isConnected();
}

// ========================================
// 断开所有连接
// ========================================
void RealTimeDate::disconnectAll()
{
    if (m_serialPort->isOpen())
    {
        m_serialViewModel->stopListening();
        m_serialPort->close();
        ui->pbtlink->setText("连接");
    }
    
    if (m_webSocketViewModel->isConnected())
    {
        m_webSocketViewModel->disconnectFromServer();
    }
    
    m_isCollecting = false;
}

// ========================================
// 根据当前模式发送命令的辅助函数
// ========================================
void RealTimeDate::sendMotorControlCommand(uint8_t fanStatus, uint8_t fanSpeed, 
                                          uint8_t pumpStatus, uint8_t lampStatus)
{
    if (m_currentMode == MODE_SERIAL && m_serialPort->isOpen())
    {
        m_serialViewModel->sendMotorControl(fanStatus, fanSpeed, pumpStatus, lampStatus);
    }
    else if (m_currentMode == MODE_WEBSOCKET && m_webSocketViewModel->isConnected())
    {
        m_webSocketViewModel->sendMotorControl(fanStatus, fanSpeed, pumpStatus, lampStatus);
    }
    else
    {
        MyToast::warning(this, "无法发送", "请先连接串口或WebSocket！");
    }
}

void RealTimeDate::sendThresholdCommand(uint8_t fanOn, uint8_t fanOff, 
                                       uint8_t pumpOn, uint8_t pumpOff,
                                       uint8_t lampOn, uint8_t lampOff)
{
    if (m_currentMode == MODE_SERIAL && m_serialPort->isOpen())
    {
        m_serialViewModel->sendThreshold(fanOn, fanOff, pumpOn, pumpOff, lampOn, lampOff);
    }
    else if (m_currentMode == MODE_WEBSOCKET && m_webSocketViewModel->isConnected())
    {
        m_webSocketViewModel->sendThreshold(fanOn, fanOff, pumpOn, pumpOff, lampOn, lampOff);
    }
    else
    {
        qDebug() << "⚠️ 未连接，无法发送阈值";
    }
}

void RealTimeDate::sendDataCollectControlCommand(bool enable)
{
    if (m_currentMode == MODE_SERIAL && m_serialPort->isOpen())
    {
        m_serialViewModel->sendDataCollectControl(enable);
    }
    else if (m_currentMode == MODE_WEBSOCKET && m_webSocketViewModel->isConnected())
    {
        m_webSocketViewModel->sendDataCollectControl(enable);
    }
    else
    {
        MyToast::warning(this, "无法发送", "请先连接串口或WebSocket！");
    }
}

void RealTimeDate::sendAutoModeControlCommand(bool enable)
{
    if (m_currentMode == MODE_SERIAL && m_serialPort->isOpen())
    {
        m_serialViewModel->sendAutoModeControl(enable);
    }
    else if (m_currentMode == MODE_WEBSOCKET && m_webSocketViewModel->isConnected())
    {
        m_webSocketViewModel->sendAutoModeControl(enable);
    }
    else
    {
        MyToast::warning(this, "无法发送", "请先连接串口或WebSocket！");
    }
}

void RealTimeDate::sendGetDataCommand(bool enable)
{
    if (m_currentMode == MODE_SERIAL && m_serialPort->isOpen())
    {
        m_serialViewModel->sendGetData(enable);
    }
    else if (m_currentMode == MODE_WEBSOCKET && m_webSocketViewModel->isConnected())
    {
        m_webSocketViewModel->sendGetData(enable);
    }
    else
    {
        MyToast::warning(this, "无法发送", "请先连接串口或WebSocket！");
    }
}
