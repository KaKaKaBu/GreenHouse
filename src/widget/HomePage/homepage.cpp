#include "HomePage.h"
#include "ui_HomePage.h"
#include <QFile>
#include <QDebug>
#include <QPushButton>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    loadStyleSheet();

    // 手动连接刷新按钮
    // connect(ui->refreshButton, &QPushButton::clicked,
    //         this, &HomePage::onRefreshButtonClicked);

    // 定时器初始化
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout,
            this, &HomePage::updateEnvironmentData);
    m_updateTimer->start(2000);

    // 初次刷新
    updateEnvironmentData();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::loadStyleSheet()
{
    QFile f(":/widget/HomePage/homepage.qss");
    if (!f.open(QFile::ReadOnly))
    {
        qWarning() << "HomePage.qss 加载失败!";
        return;
    }
    this->setStyleSheet(f.readAll());
}

void HomePage::onRefreshButtonClicked()
{
    updateEnvironmentData();
}

/**
 * 更新传感器数据（逻辑按你需要实现）
 */
void HomePage::updateEnvironmentData()
{
    // ===== 示例数据 =====
    int temp = 26;
    int hum = 58;
    int light = 320;
    int soilHum = 41;

    // ----- 写入控件 -----
    ui->tempValue->setText(QString::number(temp) + " ℃");
    ui->humidityValue->setText(QString::number(hum) + " %");
    ui->lightValue->setText(QString::number(light) + " lx");
    ui->soilValue->setText(QString::number(soilHum) + " %");

    // 状态判断
    updateCardStatus(ui->tempStatus,
                     (temp > 30 ? "偏高" :
                      temp < 15 ? "偏低" : "正常"));

    updateCardStatus(ui->humidityStatus,
                     (hum > 70 ? "偏高" :
                      hum < 40 ? "偏低" : "正常"));

    updateCardStatus(ui->lightStatus,
                     (light < 200 ? "偏弱" :
                      light > 800 ? "偏强" : "正常"));

    updateCardStatus(ui->soilStatus,
                     (soilHum < 30 ? "偏干" :
                      soilHum > 70 ? "偏湿" : "正常"));

    // 建议框
    updateSuggestion();
}

void HomePage::updateCardStatus(QLabel *statusLabel, const QString &status)
{
    statusLabel->setText("状态：" + status);
}

void HomePage::updateSuggestion()
{
    QString s;

    int temp = 26;
    int hum = 58;
    int light = 320;
    int soil = 41;

    if (temp > 30) s += "温度较高，建议通风降温。\n";
    if (temp < 15) s += "温度偏低，可适当加热。\n";

    if (hum < 40) s += "空气较干燥，可增加湿度。\n";
    if (soil < 30) s += "土壤偏干，可适当浇水。\n";

    if (s.isEmpty()) s = "环境正常，无需额外操作。";

    ui->suggestionContent->setText(s);
}
