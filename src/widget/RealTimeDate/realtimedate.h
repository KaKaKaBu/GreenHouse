//
// Created by Trubson on 2025/12/3.
// Refactored version with better separation of concerns
//

#ifndef GREENHOUSEAPP_REALTIMEDATE_REFACTORED_H
#define GREENHOUSEAPP_REALTIMEDATE_REFACTORED_H

#include <QWidget>
#include "../../model/SensorData.h"
#include "../../model/ActuatorStateData.h"
#include "../../untils/ChartManager.h"
#include "../../untils/SerialConnectionManager.h"
#include "../../untils/DeviceController.h"
#include "../../untils/ThresholdManager.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class RealTimeDate;
}

QT_END_NAMESPACE

/**
 * @brief 实时数据显示窗口 - 重构版本
 * 
 * 架构说明：
 * - UI层：只负责界面显示和用户交互
 * - ChartManager：管理图表显示
 * - SerialConnectionManager：管理串口连接
 * - DeviceController：管理设备控制
 * - ThresholdManager：管理阈值设置
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
    // ========== 数据接收回调 ==========
    void onSensorDataReceived(const SensorRecord& data);
    void onActuatorStateReceived(const ActuatorStateData& data);
    void onTimeWeatherReceived(const TimeWeatherData& data);
    
    // ========== 设备状态更新回调 ==========
    void onFanStateChanged(bool isOn);
    void onLightStateChanged(bool isOn);
    void onPumpStateChanged(bool isOn);
    void onAutoModeChanged(bool isAuto);
    
    // ========== UI更新辅助函数 ==========
    void updateDeviceButtonsUI();
    void updateThresholdUI();
    void updateSensorData();
    void loadStyleSheet();
    void connectSignals();

    // ========== UI组件 ==========
    Ui::RealTimeDate *ui;
    
    // ========== 管理器组件 ==========
    ChartManager* m_chartManager;
    SerialConnectionManager* m_serialManager;
    DeviceController* m_deviceController;
    ThresholdManager* m_thresholdManager;
    
    // ========== 状态标志 ==========
    bool m_isCollecting;
    
    // ========== 阈值滑块同步标志 ==========
    bool m_isUpdatingSlider;
    bool m_isUpdatingLineEdit;
};

#endif //GREENHOUSEAPP_REALTIMEDATE_REFACTORED_H

