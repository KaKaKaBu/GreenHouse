//
// Created by Trubson on 2025/12/3.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RealTimeDate.h" resolved

#include "realtimedate.h"
#include "ui_RealTimeDate.h"

#include <QtCharts>
#include <QChart>          // 画布
#include <QLineSeries>     // 折线数据（画笔）
#include <QChartView>      // 显示图表的窗口（把画布装起来给用户看）
#include <QValueAxis>      // 数值坐标轴（刻度）
#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsSceneWheelEvent>
#include <QDateTimeAxis>
#include <QDateTime>
#include <QDebug>

//实现鼠标动态缩放
class CustomChartView : public QChartView {
public:
    CustomChartView(QChart *chart, QWidget *parent = nullptr) : QChartView(chart, parent) {
        setRubberBand(QChartView::RectangleRubberBand);
    }

protected:
    void wheelEvent(QWheelEvent *event) override {
        // 获取当前图表
        QChart *chart = this->chart();

        // 获取滚轮角度变化
        int delta = event->angleDelta().y();

        // 设置缩放因子
        qreal factor = 1.0;
        if (delta > 0) {
            factor = 1.1;  // 放大10%
        } else {
            factor = 0.9;  // 缩小10%
        }

        // 执行缩放
        chart->zoom(factor);

        // 调用父类的wheelEvent处理其他事件
        QChartView::wheelEvent(event);
    }
    // 添加一个重置缩放的快捷键或按钮事件处理
    void resetChartZoom() {
        chart()->zoomReset();  // 恢复原始视图
    }
};

RealTimeDate::RealTimeDate(QWidget *parent) : QWidget(parent)
    , ui(new Ui::RealTimeDate)
    , isCollecting(false)   // 初始状态：未开始采集
    , dateCount(0)           // 数据点计数从0开始
{
    ui->setupUi(this);

// 初始化定时器：用于定时更新传感器数据
    dataTimer = new QTimer(this);  // 创建定时器，this表示父对象
    dataTimer->setInterval(1000);  // 设置定时器间隔为1000毫秒（1秒）

    // 连接信号和槽：当定时器超时时，调用updateSensorData函数
    connect(dataTimer, &QTimer::timeout, this, &RealTimeDate::updateSensorData);

    // 记录开始时间
    startCollectionTime = QDateTime::currentDateTime();

    //初始化折线图背景
    QChart *chart = new QChart();
    chart->setBackgroundBrush(QColor(245, 245, 245));  // 浅灰色背景
    chart->setBackgroundRoundness(10);                 // 画布圆角
    chart->setTitle("大棚内生态数据实时折线图");  // 标题内容
//    chart->createDefaultAxes();  // 自动生成坐标轴

    //创建折线
    //创建土壤湿度折线
    SoilHumiditySeries = new QLineSeries();
    SoilHumiditySeries->setName("土壤湿度(%)"); //折线名
    SoilHumiditySeries->setColor(Qt::red); //颜色
    SoilHumiditySeries->setPen(QPen(Qt::red,2)); //设置画笔 红色 二像素

    //创建空气湿度折线
    AirHumiditySeries = new QLineSeries();
    AirHumiditySeries->setName("空气湿度(%)"); //折线名
    AirHumiditySeries->setColor(Qt::blue); //颜色
    AirHumiditySeries->setPen(QPen(Qt::blue,2)); //设置画笔

    //创建光照强度折线
    SunshineSeries = new QLineSeries();
    SunshineSeries->setName("光照强度(%)"); //折线名
    SunshineSeries->setColor(QColor(160,32,240)); //颜色:紫色
    SunshineSeries->setPen(QPen(QColor(160,32,240),2)); //设置画笔

    //创建温度折线
    TemperatureSeries = new QLineSeries();
    TemperatureSeries->setName("温度(%)"); //折线名
    TemperatureSeries->setColor(Qt::green); //颜色
    TemperatureSeries->setPen(QPen(Qt::green,2)); //设置画笔

    //将折线数据系列添加到图表
    chart->addSeries(SoilHumiditySeries);
    chart->addSeries(AirHumiditySeries);
    chart->addSeries(SunshineSeries);
    chart->addSeries(TemperatureSeries);

    // 创建初始为00:00:00的时间轴
    axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText("时间");
    axisX->setTickCount(10);
    axisX->setMinorGridLineVisible(true);
    axisX->setMinorGridLinePen(QPen(QColor(180, 180, 180), 0.5, Qt::DotLine));

    // 获取Y轴--土壤湿度
    QValueAxis *axisYSoilHumidity = new QValueAxis();
    axisYSoilHumidity->setRange(0, 100);    // Y轴从10到100
    axisYSoilHumidity->setTickCount(11);      // 刻度数量
    axisYSoilHumidity->setLinePenColor(Qt::red); //轴线颜色
    axisYSoilHumidity->setLabelsColor(Qt::red); // 标签颜色
    axisYSoilHumidity->setTitleText("土壤湿度(%)");  // Y轴标题


    // 获取Y轴--空气湿度
    QValueAxis *axisYAirHumidity = new QValueAxis();
    axisYAirHumidity->setRange(0, 100);    // Y轴从10到100
    axisYAirHumidity->setTickCount(11);      // 刻度数量
    axisYAirHumidity->setLinePenColor(Qt::blue); //轴线颜色
    axisYAirHumidity->setLabelsColor(Qt::blue); // 标签颜色
    axisYAirHumidity->setTitleText("空气湿度(%)");  // Y轴标题

    // 获取Y轴--光照强度
    QValueAxis *axisYSunshine = new QValueAxis();
    axisYSunshine->setRange(0, 2000);    // Y轴从10到100
    axisYSunshine->setTickCount(11);      // 刻度数量
    axisYSunshine->setLinePenColor(QColor(160,32,240)); //轴线颜色:紫色
    axisYSunshine->setLabelsColor(QColor(160,32,240)); // 标签颜色
    axisYSunshine->setTitleText("光照强度(%)");  // Y轴标题

    // 获取Y轴--温度
    QValueAxis *axisYTemperature = new QValueAxis();
    axisYTemperature->setRange(0, 50);    // Y轴
    axisYTemperature->setTickCount(11);      // 刻度数量
    axisYTemperature->setLinePenColor(Qt::green); //轴线颜色
    axisYTemperature->setLabelsColor(Qt::green); // 标签颜色
    axisYTemperature->setTitleText("温度(°C)");  // Y轴标题

    //把轴放图表里
    // x轴放下面
    chart->addAxis(axisX, Qt::AlignBottom);  // X轴在底部

    // Y轴添加到左侧（可以叠加）
    chart->addAxis(axisYSoilHumidity, Qt::AlignLeft);
    chart->addAxis(axisYAirHumidity, Qt::AlignLeft);

    // Y轴添加到右侧
    chart->addAxis(axisYSunshine, Qt::AlignRight);
    chart->addAxis(axisYTemperature, Qt::AlignRight);

    //关联折线与轴线
    //土壤湿度
    SoilHumiditySeries->attachAxis(axisX);
    SoilHumiditySeries->attachAxis(axisYSoilHumidity);

    //空气湿度
    AirHumiditySeries->attachAxis(axisX);
    AirHumiditySeries->attachAxis(axisYAirHumidity);

    //光照强度
    SunshineSeries->attachAxis(axisX);
    SunshineSeries->attachAxis(axisYSunshine);

    //温度
    TemperatureSeries->attachAxis(axisX);
    TemperatureSeries->attachAxis(axisYTemperature);

    // 设置图例位置
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // 使用自定义的ChartView
    CustomChartView *chartView = new CustomChartView(chart);
    // 设置图表视图的交互模式
    chartView->setRubberBand(QChartView::RectangleRubberBand);  // 保留矩形选择功能
    chartView->setDragMode(QChartView::ScrollHandDrag);        // 保留拖拽功能

    // 重要：启用图表的鼠标追踪
    chartView->setMouseTracking(true);

    // 允许图表平移
    chartView->setDragMode(QChartView::ScrollHandDrag);

    // 启用抗锯齿渲染，提高图表显示质量
    chartView->setRenderHint(QPainter::Antialiasing);

    this->ui->frame_2->setLayout(new QVBoxLayout());
    // 设置布局属性
    this->ui->frame_2->layout()->setContentsMargins(0, 0, 0, 0);
    this->ui->frame_2->layout()->setSpacing(0);

    // 添加图表视图到布局
    this->ui->frame_2->layout()->addWidget(chartView);

    // 初始化采集状态
    isCollecting = false;

    //设置按钮初始状态
    //默认风扇为工作结束'rbtAir_close'，无法用鼠标切换
    ui->rbtAir_open->setEnabled(!isLocked);
    ui->rbtAir_close->setEnabled(!isLocked);
    ui->rbtAir_open->setChecked(false);
    ui->rbtAir_close->setChecked(true);

    //默认补光灯泡为已结束'rbtLight_close'，无法用鼠标切换
    ui->rbtLight_open->setEnabled(!isLocked);
    ui->rbtLight_close->setEnabled(!isLocked);
    ui->rbtLight_open->setChecked(false);
    ui->rbtLight_close->setChecked(true);

    //默认抽水泵为浇灌结束'rbtWater_close',无法用鼠标切换
    ui->rbtWater_open->setEnabled(!isLocked);
    ui->rbtWater_close->setEnabled(!isLocked);
    ui->rbtWater_open->setChecked(false);
    ui->rbtWater_close->setChecked(true);
}

RealTimeDate::~RealTimeDate() {
    delete ui;
}

void RealTimeDate::updateSensorData()
{
    // 生成新的模拟数据
    generateRandomSensorData();

    //获取当前时间
    QDateTime timeNow = QDateTime::currentDateTime();

    // 使用毫秒时间戳作为X坐标，下位机上传的值作为Y坐标
    TemperatureSeries->append(timeNow.toMSecsSinceEpoch(),temperatureData.last());
    SoilHumiditySeries->append(timeNow.toMSecsSinceEpoch(),soilhumidityData.last());
    AirHumiditySeries->append(timeNow.toMSecsSinceEpoch(),airhumidityData.last());
    SunshineSeries->append(timeNow.toMSecsSinceEpoch(),sunshineData.last());

    // 存储时间戳
    timeData.append(timeNow);
    dateCount++;  // 增加数据点计数

    // === 动态调整X轴范围：显示最近30秒的数据 ===
    if (dateCount > 1) {  // 确保有足够的数据点
        QDateTime minTime = timeNow.addSecs(-30);  // 30秒前
        QDateTime maxTime = timeNow.addSecs(2);    // 2秒后（留出一些空间）

        axisX->setRange(minTime, maxTime);  // 更新X轴范围
    }

    // === 限制数据点数量，防止内存过度增长 ===
    const int MAX_POINTS = 100;  // 最大显示100个数据点
    if (TemperatureSeries->count() > MAX_POINTS) {
        // 移除旧数据点，只保留最新的MAX_POINTS个点
        int removeCount = TemperatureSeries->count() - MAX_POINTS;
        TemperatureSeries->removePoints(0, removeCount);
        AirHumiditySeries->removePoints(0, removeCount);
        SunshineSeries->removePoints(0, removeCount);
        SoilHumiditySeries->removePoints(0, removeCount);
    }

    // 更新图表显示
    this->ui->frame_2->update();
}

void RealTimeDate::generateRandomSensorData()
{
    // 使用静态变量保存上一次的值，实现数据的连续性

    // === 模拟温度数据 (15-35°C 范围内缓慢变化) ===
    static double lastTemp = 25.0;  // 初始温度25°C
    // 生成-0.5到0.5之间的随机变化量，然后乘以2得到-1到1的变化
    double tempChange = (QRandomGenerator::global()->generateDouble() - 0.5) * 2.0;
    // 计算新温度，并限制在15-35°C范围内
    double newTemp = qBound(15.0, lastTemp + tempChange, 35.0);
    temperatureData.append(newTemp);  // 存储新数据
    lastTemp = newTemp;               // 更新上一次的值

    // === 模拟土壤湿度数据 (30-80% 范围内缓慢变化) ===
    static double lastsoilHumidity = 60.0;  // 初始湿度60%
    double soilhumidityChange = (QRandomGenerator::global()->generateDouble() - 0.5) * 5.0;
    double newsoilHumidity = qBound(30.0, lastsoilHumidity + soilhumidityChange, 80.0);
    soilhumidityData.append(newsoilHumidity);
    lastsoilHumidity = newsoilHumidity;

    // === 模拟土壤湿度数据 (30-80% 范围内缓慢变化) ===
    static double lastairHumidity = 50.0;  // 初始湿度50%
    double airhumidityChange = (QRandomGenerator::global()->generateDouble() - 0.5) * 5.0;
    double newairHumidity = qBound(30.0, lastairHumidity + airhumidityChange, 80.0);
    airhumidityData.append(newairHumidity);
    lastairHumidity = newairHumidity;

    // === 模拟光照数据 (0-2000 Lux，可能有较大波动) ===
    static double lastLight = 500.0;  // 初始光照500 Lux
    double lightChange = (QRandomGenerator::global()->generateDouble() - 0.5) * 200.0;
    double newLight = qBound(0.0, lastLight + lightChange, 2000.0);
    sunshineData.append(newLight);
    lastLight = newLight;

    // 输出新生成的数据到调试窗口
    qDebug() << QString("新数据 - 温度: %1°C, 湿度: %2%, 光照: %3Lux")
                .arg(newTemp, 0, 'f', 1)    // 格式化为1位小数
                .arg(newsoilHumidity, 0, 'f', 1)
                .arg(newairHumidity, 0, 'f', 1)
                .arg(newLight, 0, 'f', 0);  // 光照取整
}

void RealTimeDate::on_pbtStart_clicked()
{
    if (!isCollecting) {          // 如果当前没有在采集
        dataTimer->start();       // 启动定时器
        isCollecting = true;      // 更新状态标志
        qDebug() << "开始数据采集...";  // 输出调试信息
    }
}

void RealTimeDate::on_pbtEnd_clicked()
{
    if (isCollecting) {           // 如果当前正在采集
        dataTimer->stop();        // 停止定时器
        isCollecting = false;     // 更新状态标志
        qDebug() << "停止数据采集...";
    }
}


void RealTimeDate::on_pbtClaer_clicked()
{
    // 清除所有数据系列
    TemperatureSeries->clear();  // 清空温度数据点
    AirHumiditySeries->clear();     // 清空湿度数据点
    SunshineSeries->clear();        // 清空光照数据点
    SoilHumiditySeries->clear();

    // 清空存储的历史数据
    timeData.clear();
    sunshineData.clear();
    soilhumidityData.clear();
    airhumidityData.clear();
    temperatureData.clear();

    // 重置计数器和开始时间
    dateCount = 0;
    startCollectionTime = QDateTime::currentDateTime();

    qDebug() << "数据已清除";
}

void RealTimeDate::on_pbtAir_clicked()
{
    if(!ui->pbtAir->isEnabled()){
        qDebug() << "自动通风按钮被锁定！";
        return;
    }else

    airIsOpen = !airIsOpen;  // 取反

    ui->rbtAir_close->setText("工作结束");
    ui->rbtAir_open->setChecked(airIsOpen);
    ui->rbtAir_close->setChecked(!airIsOpen);

    qDebug() << "当前选中：" << (airIsOpen ? "rbtAir_open" : "rbtAir_close");
}

void RealTimeDate::on_pbtLight_clicked()
{
    if(!ui->pbtLight->isEnabled()){
        qDebug() << "自动补光按钮被锁定！";
        return;
    }else

    lightIsOpen = !lightIsOpen;  // 取反

    ui->rbtLight_close->setText("灯光关闭");
    ui->rbtLight_open->setChecked(lightIsOpen);
    ui->rbtLight_close->setChecked(!lightIsOpen);

    qDebug() << "当前选中：" << (lightIsOpen ? "rbtLight_open" : "rbtLight_close");
}

void RealTimeDate::on_pbtWater_clicked()
{
    if(!ui->pbtWater->isEnabled()){
        qDebug() << "自动浇灌按钮被锁定！";
        return;
    }else

    waterIsOpen = !waterIsOpen;  // 取反

    ui->rbtWater_close->setText("浇灌完成");
    ui->rbtWater_open->setChecked(waterIsOpen);
    ui->rbtWater_close->setChecked(!waterIsOpen);

    qDebug() << "当前选中：" << (waterIsOpen ? "rbtWater_open" : "rbtWater_close");
}

void RealTimeDate::on_hsr_High_Temperature_sliderReleased()
{
    // 使用标志位防止递归调用
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    //更新文本框,获取值
    value = ui->hsr_High_Temperature->value();
    ui->let_High_Temperature->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "High_Temperature_Slider值改变：" << value;
}

void RealTimeDate::on_let_High_Temperature_textChanged(const QString &arg1)
{
    // 使用标志位防止递归调用
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);

    if(ok){
        isUpdatingSlider = true;

        ui->hsr_High_Temperature ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_Low_Temperature_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_Low_Temperature->value();
    ui->let_Low_Temperature->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "Low_Temperature_Slider值改变：" << value;
}

void RealTimeDate::on_let_Low_Temperature_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_Low_Temperature ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_High_Air_Humidity_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_High_Air_Humidity->value();
    ui->let_High_Air_Humidity->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "High_Air_Humidity_Slider值改变：" << value;
}

void RealTimeDate::on_let_High_Air_Humidity_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_High_Air_Humidity ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_Low_Air_Humidity_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_Low_Air_Humidity->value();
    ui->let_Low_Air_Humidity->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "Low_Air_Humidity_Slider值改变：" << value;
}

void RealTimeDate::on_let_Low_Air_Humidity_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_Low_Air_Humidity ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_High_Light_Intensity_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_High_Light_Intensity->value();
    ui->let_High_Light_Intensity->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "High_Light_Intensity_Slider值改变：" << value;
}

void RealTimeDate::on_let_High_Light_Intensity_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_High_Light_Intensity ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_Low_Light_Intensity_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_Low_Light_Intensity->value();
    ui->let_Low_Light_Intensity->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "Low_Light_Intensity_Slider值改变：" << value;
}

void RealTimeDate::on_let_Low_Light_Intensity_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_Low_Light_Intensity ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_High_Soil_Moisture_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_High_Soil_Moisture->value();
    ui->let_High_Soil_Moisture->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "High_Soil_Moisture_Slider值改变：" << value;
}

void RealTimeDate::on_let_High_Soil_Moisture_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_High_Soil_Moisture ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_hsr_Low_Soil_Moisture_sliderReleased()
{
    if (isUpdatingSlider) {
        return;
    }
    isUpdatingLineEdit = true;
    value = ui->hsr_Low_Soil_Moisture->value();
    ui->let_Low_Soil_Moisture->setText(QString::number(value));
    isUpdatingLineEdit = false;
    qDebug() << "Low_Soil_Moisture_Slider值改变：" << value;
}

void RealTimeDate::on_let_Low_Soil_Moisture_textChanged(const QString &arg1)
{
    if(isUpdatingLineEdit){
        return;
    }
    if(arg1.isEmpty()){
        return;
    }
    bool ok;
    int value = arg1.toInt(&ok);
    if(ok){
        isUpdatingSlider = true;

        ui->hsr_Low_Soil_Moisture ->setValue(value);
        isUpdatingSlider = false;
    }else {
        qDebug() << "输入无效：" << arg1;
    }
}

void RealTimeDate::on_Automatic_clicked()
{
    isLocked = !isLocked; //翻转
    //改变按钮颜色
    QString style = isLocked ? "" : "QPushButton { color: #ffffff; background-color: #000000; }";
    ui->pbtAir->setStyleSheet(style);
    ui->pbtLight->setStyleSheet(style);
    ui->pbtWater->setStyleSheet(style);

    //手动按钮无法工作
    ui->pbtAir->setEnabled(isLocked);
    ui->pbtLight->setEnabled(isLocked);
    ui->pbtWater->setEnabled(isLocked);

    //状态回归默认值
    ui->rbtAir_open->setChecked(false);
    ui->rbtAir_close->setChecked(true);
    ui->rbtAir_close->setText("已自动");

    ui->rbtLight_open->setChecked(false);
    ui->rbtLight_close->setChecked(true);
    ui->rbtLight_close->setText("已自动");

    ui->rbtWater_open->setChecked(false);
    ui->rbtWater_close->setChecked(true);
    ui->rbtWater_close->setText("已自动");
}
