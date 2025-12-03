//
// Created by Trubson on 2025/12/3.
//

#ifndef GREENHOUSEAPP_REALTIMEDATE_H
#define GREENHOUSEAPP_REALTIMEDATE_H

#include <QWidget>

#include <QtCharts>
#include <QChart>          // 画布
#include <QLineSeries>     // 折线数据（画笔）
#include <QChartView>      // 显示图表的窗口（把画布装起来给用户看）
#include <QValueAxis>      // 数值坐标轴（刻度）
#include <QDateTimeAxis>
#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE

namespace Ui {
    class RealTimeDate;
}

QT_END_NAMESPACE

class RealTimeDate : public QWidget {
    Q_OBJECT

public:
    explicit RealTimeDate(QWidget *parent = nullptr);

    ~RealTimeDate() override;

private slots:
    //采集按钮
    void on_pbtStart_clicked();

    //结束采集按钮
    void on_pbtEnd_clicked();

    //清除采集数据按钮
    void on_pbtClaer_clicked();

    //定时器函数
    void updateSensorData();

    //手动打开风扇按钮
    void on_pbtAir_clicked();

    //手动开灯补光按钮
    void on_pbtLight_clicked();

    //手动抽水浇灌按钮
    void on_pbtWater_clicked();

    //改变温度阈值（峰值）
    void on_hsr_High_Temperature_sliderReleased();
    void on_let_High_Temperature_textChanged(const QString &arg1);

    //改变温度阈值（低值）
    void on_hsr_Low_Temperature_sliderReleased();
    void on_let_Low_Temperature_textChanged(const QString &arg1);

    //改变空气湿度阈值（峰值）
    void on_hsr_High_Air_Humidity_sliderReleased();
    void on_let_High_Air_Humidity_textChanged(const QString &arg1);

    //改变空气湿度阈值（低值）
    void on_hsr_Low_Air_Humidity_sliderReleased();
    void on_let_Low_Air_Humidity_textChanged(const QString &arg1);

    //改变光照强度阈值（峰值）
    void on_hsr_High_Light_Intensity_sliderReleased();
    void on_let_High_Light_Intensity_textChanged(const QString &arg1);

    //改变光照强度阈值（低值）
    void on_hsr_Low_Light_Intensity_sliderReleased();
    void on_let_Low_Light_Intensity_textChanged(const QString &arg1);

    //改变土壤湿度阈值（峰值）
    void on_hsr_High_Soil_Moisture_sliderReleased();
    void on_let_High_Soil_Moisture_textChanged(const QString &arg1);

    //改变土壤湿度阈值（低值）
    void on_hsr_Low_Soil_Moisture_sliderReleased();
    void on_let_Low_Soil_Moisture_textChanged(const QString &arg1);

    //自动模式
    void on_Automatic_clicked();

private:
    Ui::RealTimeDate *ui;

    //创建X时间轴
    QDateTimeAxis * axisX;

    //创建折线
    QLineSeries *SoilHumiditySeries;
    QLineSeries *SunshineSeries;
    QLineSeries *TemperatureSeries;
    QLineSeries *AirHumiditySeries;

    //创建时间模型
    QTimer *dataTimer;
    bool isCollecting;
    QDateTime startCollectionTime;
    int dateCount;

    //存储采集数据
    QVector<QDateTime> timeData;       // 存储每个数据点的时间戳
    QVector<double> temperatureData;   // 存储温度历史数据
    QVector<double> airhumidityData;      // 存储空气湿度历史数据
    QVector<double> sunshineData;         // 存储光照历史数据
    QVector<double> soilhumidityData;    //存储土壤湿度历史数据

    //定义函数
    void generateRandomSensorData();  // 生成模拟传感器数据

    //按钮控制（左边三个按钮）
    bool airIsOpen = false;
    bool lightIsOpen = false;
    bool waterIsOpen = false;

    //控制按钮
    bool isLocked = true;

    //为Slider和文本框添加标志位防止递归调用
    bool isUpdatingSlider = false;
    bool isUpdatingLineEdit = false;
    int value = 0; //获取Slider拖动停止后的值
};


#endif //GREENHOUSEAPP_REALTIMEDATE_H