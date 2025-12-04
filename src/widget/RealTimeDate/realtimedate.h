//
// Created by Trubson on 2025/12/3.
// MVVM Architecture Version
//

#ifndef GREENHOUSEAPP_REALTIMEDATE_H
#define GREENHOUSEAPP_REALTIMEDATE_H

#include <QWidget>
#include <QSerialPort>
#include <QChart>
#include <QSerialPortInfo>
#include <QChartView>
#include <QDateTimeAxis>
#include <QLineSeries>
#include <QValueAxis>

#include "../../model/SensorData.h"
#include "../../model/ActuatorStateData.h"
#include "../../viewmodel/SerialViewModel.h"
#include "../../viewmodel/SensorViewModel.h"
#include "../../viewmodel/ControlViewModel.h"
#include "../../viewmodel/ChartViewModel.h"
#include "../../viewmodel/SettingViewModel.h"

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui {
    class RealTimeDate;
}
QT_END_NAMESPACE

/**
 * @brief 实时数据显示窗口 - MVVM 架构版本
 * 
 * 架构说明：
 * - View 层：只负责界面显示和用户交互
 * - ViewModel 层：SerialViewModel, SensorViewModel, ControlViewModel, ChartViewModel, SettingViewModel
 * - Model 层：SensorRecord, ActuatorStateData, TimeWeatherData
 * 
 * 数据流：
 * - 接收数据：Model → ViewModel → View
 * - 发送命令：View → ViewModel → Model
 */
class RealTimeDate : public QWidget {
    Q_OBJECT

public:
    explicit RealTimeDate(QWidget *parent = nullptr);
    ~RealTimeDate() override;

private slots:
    // ========== 串口连接 ==========
    void on_pbtlink_clicked();
    
    // ========== 数据采集 ==========
    void on_pbtStart_clicked();
    void on_pbtEnd_clicked();
    void on_pbtClaer_clicked();
    
    // ========== 设备控制 ==========
    void on_pbtAir_clicked();
    void on_pbtLight_clicked();
    void on_pbtWater_clicked();
    void on_Automatic_clicked();
    
    // ========== 阈值设置 ==========
    void on_hsr_High_Temperature_sliderReleased();
    void on_let_High_Temperature_textChanged(const QString &arg1);
    void on_hsr_Low_Temperature_sliderReleased();
    void on_let_Low_Temperature_textChanged(const QString &arg1);
    
    void on_hsr_High_Air_Humidity_sliderReleased();
    void on_let_High_Air_Humidity_textChanged(const QString &arg1);
    void on_hsr_Low_Air_Humidity_sliderReleased();
    void on_let_Low_Air_Humidity_textChanged(const QString &arg1);
    
    void on_hsr_High_Light_Intensity_sliderReleased();
    void on_let_High_Light_Intensity_textChanged(const QString &arg1);
    void on_hsr_Low_Light_Intensity_sliderReleased();
    void on_let_Low_Light_Intensity_textChanged(const QString &arg1);
    
    void on_hsr_High_Soil_Moisture_sliderReleased();
    void on_let_High_Soil_Moisture_textChanged(const QString &arg1);
    void on_hsr_Low_Soil_Moisture_sliderReleased();
    void on_let_Low_Soil_Moisture_textChanged(const QString &arg1);

private:
    // ========== ViewModel 数据接收回调 ==========
    void onSensorDataReceived(const SensorRecord& data);
    void onActuatorStateReceived(const ActuatorStateData& data);
    void onTimeWeatherReceived(const TimeWeatherData& data);
    void onHeartBeatReceived();
    
    // ========== 初始化函数 ==========
    void setupViewModels();          // 创建 ViewModel 实例
    void connectViewModelSignals();  // 连接 ViewModel 信号
    void initializeUI();             // 初始化 UI
    void initializeChart();          // 初始化图表
    
    // ========== UI 更新辅助函数 ==========
    void updateChartDisplay(const SensorRecord& data);
    void updateSensorLabels(const SensorRecord& data);
    void updateDeviceButtonsUI();
    void updateThresholdUI();
    void loadStyleSheet();
    void sendAllThresholdsToDevice();   // 发送所有阈值到下位机

    // ========== UI 组件 ==========
    Ui::RealTimeDate *ui;
    QChart* m_chart;
    QChartView* m_chartView;
    
    // ========== 图表序列 ==========
    QLineSeries* m_temperatureSeries;     // 温度曲线
    QLineSeries* m_airHumiditySeries;     // 空气湿度曲线
    QLineSeries* m_soilHumiditySeries;    // 土壤湿度曲线
    QLineSeries* m_lightIntensitySeries;  // 光照强度曲线
    
    QDateTimeAxis* m_axisX;               // X轴（时间）
    QValueAxis* m_axisY;                  // Y轴（数值）
    
    // ========== ViewModel 实例 (MVVM 架构核心) ==========
    SerialViewModel* m_serialViewModel;      // 串口通信 ViewModel
    SensorViewModel* m_sensorViewModel;      // 传感器数据 ViewModel
    ControlViewModel* m_controlViewModel;    // 设备控制 ViewModel
    ChartViewModel* m_chartViewModel;        // 图表数据 ViewModel
    SettingViewModel* m_settingViewModel;    // 设置管理 ViewModel
    
    // ========== 底层依赖 ==========
    QSerialPort* m_serialPort;
    
    // ========== 状态标志 ==========
    bool m_isCollecting;
    bool m_isUpdatingSlider;
    bool m_isUpdatingLineEdit;
};

#endif //GREENHOUSEAPP_REALTIMEDATE_H
