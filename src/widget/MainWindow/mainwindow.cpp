#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // // 初始化串口
    // QSerialPort* serial = new QSerialPort(this);
    // serial->setPortName("COM3");
    // serial->setBaudRate(115200);
    // serial->open(QIODevice::ReadWrite);
    //
    // // 创建 ViewModel
    // auto serialVM = new SerialViewModel(serial, this);
    // auto sensorVM = new SensorViewModel(this); // 可用于 UI 绑定
    //
    // // 连接：当收到数据 → 插入数据库
    // connect(serialVM, &SerialViewModel::sensorDataReceived,
    //         [](const SensorRecord& record) {
    //     // 调用你的数据库插入函数
    //     insertSensorRecordIntoDB(record); // 你需要实现这个函数
    // });
    //
    // // （可选）也连接到 SensorViewModel 用于 UI 刷新
    // connect(serialVM, &SerialViewModel::sensorDataReceived,
    //         sensorVM, &SensorViewModel::sensorDataUpdated);
}

MainWindow::~MainWindow()
{
    delete ui;
}
