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



/**
 * 更新传感器数据（逻辑按你需要实现）
 */
void HomePage::updateEnvironmentData(const SensorRecord& data)
{

    // ----- 写入控件 -----

    ui->tempValue->setText(QString::number(data.air_temp)+"℃");
    ui->humidityValue->setText(QString::number(data.air_humid) + " %");
    ui->lightValue->setText(QString::number(data.light_intensity) + " %");
    ui->soilValue->setText(QString::number(data.soil_humid) + " %");

    // 状态判断
    updateCardStatus(ui->tempStatus,
                     (data.air_temp > 30 ? "偏高" :
                      data.air_temp < 15 ? "偏低" : "正常"));

    updateCardStatus(ui->humidityStatus,
                     (data.air_humid > 70 ? "偏高" :
                      data.air_humid < 40 ? "偏低" : "正常"));

    updateCardStatus(ui->lightStatus,
                     (data.light_intensity < 200 ? "偏弱" :
                      data.light_intensity > 800 ? "偏强" : "正常"));

    updateCardStatus(ui->soilStatus,
                     (data.soil_humid < 30 ? "偏干" :
                      data.soil_humid > 70 ? "偏湿" : "正常"));

    // 建议框
    updateSuggestion(data);
}

void HomePage::updateCardStatus(QLabel *statusLabel, const QString &status)
{
    statusLabel->setText("状态：" + status);
}

void HomePage::updateSuggestion(const SensorRecord& data)
{
    QString s;


    if (data.air_temp > 30) s += "温度较高，建议通风降温。\n";
    if (data.air_temp < 15) s += "温度偏低，可适当加热。\n";

    if (data.air_humid < 40) s += "空气较干燥，可增加湿度。\n";
    if (data.soil_humid < 30) s += "土壤偏干，可适当浇水。\n";

    if (data.light_intensity<30) s+="光线较暗，请及时开灯\n";

    if (s.isEmpty()) s = "环境正常，无需额外操作。";

    ui->suggestionContent->setText(s);
}
