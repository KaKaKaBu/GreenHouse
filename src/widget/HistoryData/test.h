//
// Created by 刘慧敏 on 2025/12/2.
//

#ifndef GREENHOUSE_TEST_H
#define GREENHOUSE_TEST_H

#include <qdatetime.h>
#include <QWidget>
#include <QtCharts>
QVector<QPair<QDateTime, double>> generateDate(const QDateTime &start, const QDateTime &end, int count = 50);
QVector<QPair<QDateTime, double>> generateDate(const QDateTime &start, const QDateTime &end, double minVal, double maxVal, int count = 50);
class QVBoxLayout;

namespace QtCharts {
    class QChartView;
}

QT_BEGIN_NAMESPACE

namespace Ui {
    class test;
}

QT_END_NAMESPACE

class test : public QWidget {
    Q_OBJECT

public:
    explicit test(QWidget *parent = nullptr);
    ~test() override;
protected:
    // 声明事件过滤器
    bool eventFilter(QObject *watched, QEvent *event) override;
private slots:
    void on_pushQuery_clicked();
    void on_pushClear_clicked();
    void on_pushClearHistory_clicked();
private:
    Ui::test *ui;
    // ChartView 指针
    QChartView *airChartView = nullptr;   // 空气温湿度图表视图
    QChartView *soilChartView = nullptr;  // 土壤湿度图表视图
    QChartView *lightChartView = nullptr; // 光照强度图表视图

    void updateChartData(bool resetZoom = false);
    // Series 指针（用于更新数据）
    QLineSeries *tempSeries = nullptr;
    QLineSeries *humiSeries = nullptr;
    QLineSeries *lightSeries = nullptr;
    QLineSeries *soilSeries = nullptr;

    void initAirChart();
    void initLightChart();
    void initSoilChart();
};

#endif //GREENHOUSE_TEST_H