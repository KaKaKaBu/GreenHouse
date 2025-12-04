#include "test.h"
#include "ui_test.h"
#include "../src/model/Database/Database.h"
#include "model/Database/Database.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QPainter>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QVBoxLayout>
QT_CHARTS_USE_NAMESPACE

test::test(QWidget *parent) : QWidget(parent), ui(new Ui::test) {
    ui->setupUi(this);
    //设置默认时间范围（最近一天）
    ui->dateStartTime->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->dateOverTime->setDateTime(QDateTime::currentDateTime());
    //查询
    connect(ui->pushQuery,&QPushButton::clicked,this,&test::on_pushQuery_clicked);
    //重置
    connect(ui->pushClear,&QPushButton::clicked,this,&test::on_pushClear_clicked);
    //删除历史记录
    connect(ui->pushClearHistory,&QPushButton::clicked,this,&test::on_pushClearHistory_clicked);
    initAirChart();
    initSoilChart();
    initLightChart();
    // 加载初始数据
    updateChartData();
}
//初始化空气温湿度图表
void test::initAirChart() {
    if (airChartView) return;

    QVBoxLayout *tabLayout = new QVBoxLayout(ui->tabAir);
    tabLayout->setContentsMargins(0, 0, 0, 0);

    QChart *chart = new QChart();
    chart->setTitle("空气温湿度变化");

    tempSeries = new QLineSeries();
    humiSeries = new QLineSeries();
    tempSeries->setName("温度(摄氏度)");
    humiSeries->setName("湿度（%）");

    //设置X轴（时间轴）
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    tempSeries->attachAxis(axisX);
    humiSeries->attachAxis(axisX);

    // Y 轴 1：湿度（左）—— 保留网格线
    QValueAxis *axisYHumi = new QValueAxis();
    axisYHumi->setRange(0, 100);
    axisYHumi->setTickCount(11);
    axisYHumi->setLabelFormat("%d%%");
    axisYHumi->setTitleText("湿度 (%)");
    axisYHumi->setGridLineVisible(true);
    chart->addAxis(axisYHumi, Qt::AlignLeft);
    humiSeries->attachAxis(axisYHumi);

    // Y 轴 2：温度（右）—— 关闭网格线，只显示刻度
    QValueAxis *axisYTemp = new QValueAxis();
    axisYTemp->setRange(-20, 60);
    axisYTemp->setTickCount(11);
    axisYTemp->setLabelFormat("%.0f");
    axisYTemp->setTitleText("温度 (°C)");
    axisYTemp->setGridLineVisible(false);
    chart->addAxis(axisYTemp, Qt::AlignRight);
    tempSeries->attachAxis(axisYTemp);

    airChartView = new QChartView(chart);
    airChartView->setRenderHint(QPainter::Antialiasing);
    airChartView->setInteractive(true);
    airChartView->setRubberBand(QChartView::HorizontalRubberBand);
    airChartView->installEventFilter(this);

    tabLayout->addWidget(airChartView);
}
//初始化光照强度图表
void test::initLightChart() {
    // 删除旧布局（不管是什么类型）
    QLayout *oldLayout = ui->tabLight->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget(); // 删除子控件
            delete item;           // 删除布局项
        }
        delete oldLayout; // 删除旧布局本身
    }

    // 创建新布局
    QVBoxLayout *tabLayout = new QVBoxLayout(ui->tabLight);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    // ==========先初始化lightSeries ==========
    lightSeries = new QLineSeries();
    lightSeries->setName("光照强度");
    //创建图表
    QChart *chart=new QChart();
    chart->setTitle("光照强度变化");

    //添加系列到图表
    chart->addSeries(lightSeries);

    //设置X轴（时间轴）
    QDateTimeAxis *axisX=new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("时间");
    chart->addAxis(axisX,Qt::AlignBottom);
    lightSeries->attachAxis(axisX);

    // Y 轴 光照强度
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 1000);
    axisY->setTickCount(11);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("光照强度 ");
    axisY->setGridLineVisible(true);      // ✅ 显示网格线
    chart->addAxis(axisY, Qt::AlignLeft);
    lightSeries->attachAxis(axisY);

    //创建chartView
    QChartView *chartView=new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setInteractive(true); // 启用图表交互
    chartView->setRubberBand(QChartView::HorizontalRubberBand); // 支持框选放大
    chartView->installEventFilter(this); // 安装事件过滤器

    //直接添加到新建的布局
    tabLayout->addWidget(chartView);
    //保存指针
    lightChartView=chartView;
}
//初始化土壤强度图表
void test::initSoilChart() {
    // 复用或创建布局（避免重复 new）
    if (!ui->tabSoil->layout()) {
        QVBoxLayout *layout = new QVBoxLayout(ui->tabSoil);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    QVBoxLayout *tabLayout = qobject_cast<QVBoxLayout*>(ui->tabSoil->layout());

    // 清除旧图表（如果存在）
    QLayoutItem* item;
    while ((item = tabLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    // ========== 关键修复：先初始化soilSeries ==========
    soilSeries = new QLineSeries();
    soilSeries->setName("土壤湿度（%）");
    //创建图表
    QChart *chart=new QChart();
    chart->setTitle("土壤湿度变化");

    //添加系列到图表
    chart->addSeries(soilSeries);

    //设置X轴（时间轴）
    QDateTimeAxis *axisX=new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("时间");
    chart->addAxis(axisX,Qt::AlignBottom);
    soilSeries->attachAxis(axisX);

    // Y 轴 光照强度
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTickCount(11);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("土壤湿度 ");
    axisY->setGridLineVisible(true);      // ✅ 显示网格线
    chart->addAxis(axisY, Qt::AlignLeft);
    soilSeries->attachAxis(axisY);

    //创建chartView
    QChartView *chartView=new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setInteractive(true); // 启用图表交互
    chartView->setRubberBand(QChartView::HorizontalRubberBand); // 支持框选放大
    chartView->installEventFilter(this); // 安装事件过滤器

    //直接添加到新建的布局
    tabLayout->addWidget(chartView);
    //保存指针
    soilChartView=chartView;
}
void test::updateChartData(bool resetZoom) {
    QDateTime start = ui->dateStartTime->dateTime();
    QDateTime end = ui->dateOverTime->dateTime();

    if (start >= end) {
        QMessageBox::warning(this, "时间错误", "开始时间必须早于结束时间！");
        return;
    }

    // 查询数据库数据
    std::string startTime = start.toString("yyyy-MM-dd HH:mm:ss").toStdString();
    std::string endTime = end.toString("yyyy-MM-dd HH:mm:ss").toStdString();

    std::vector<GreenData> dataList;
    bool querySuccess = Database::queryByTime(startTime, endTime, dataList);

    if (!querySuccess) {
        QMessageBox::critical(this, "错误", "数据库查询失败！");
        return;
    }

    // 更新图表数据系列
    auto updateSeries = [](QLineSeries* series, const std::vector<GreenData>& data,
                          std::function<double(const GreenData&)> getValue) {
        series->clear();
        for (const auto& d : data) {
            QDateTime time = QDateTime::fromString(QString::fromStdString(d.record_time),
                                                 "yyyy-MM-dd HH:mm:ss");
            if (time.isValid()) {
                series->append(time.toMSecsSinceEpoch(), getValue(d));
            }
        }
    };

    if (!dataList.empty()) {
        updateSeries(tempSeries, dataList, [](const GreenData& d) { return d.air_temp; });
        updateSeries(humiSeries, dataList, [](const GreenData& d) { return d.air_humid; });
        updateSeries(lightSeries, dataList, [](const GreenData& d) { return d.light_intensity; });
        updateSeries(soilSeries, dataList, [](const GreenData& d) { return d.soil_humid; });
    } else {
        // 无数据时清空图表
        tempSeries->clear();
        humiSeries->clear();
        lightSeries->clear();
        soilSeries->clear();
        QMessageBox::information(this, "提示", "所选时间范围内无数据！");
    }

    // === 更新所有 X 轴范围 ===
    auto updateAxis = [&](QChartView* view) {
        if (!view) return;
        auto axes = view->chart()->axes(Qt::Horizontal);
        if (!axes.isEmpty()) {
            QDateTimeAxis* axisX = qobject_cast<QDateTimeAxis*>(axes.at(0));
            if (axisX) {
                axisX->setRange(start, end);
                // 根据时间跨度自动调整格式
                qint64 secs = start.secsTo(end);
                if (secs < 3600) {
                    axisX->setFormat("HH:mm:ss");
                } else if (secs < 86400) {
                    axisX->setFormat("MM-dd HH:mm");
                } else {
                    axisX->setFormat("yyyy-MM-dd");
                }
            }
        }
    };

    updateAxis(airChartView);
    updateAxis(lightChartView);
    updateAxis(soilChartView);

    // === 是否重置缩放 ===
    if (resetZoom) {
        if (airChartView) airChartView->chart()->zoomReset();
        if (lightChartView) lightChartView->chart()->zoomReset();
        if (soilChartView) soilChartView->chart()->zoomReset();
    }
}

test::~test() {
    delete ui;
}

bool test::eventFilter(QObject *watched, QEvent *event) {
    // 只处理 airChartView、lightChartView 或 soilChartView 的滚轮事件
    if ((watched == airChartView || watched == lightChartView || watched == soilChartView)
        && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

        // 动态获取当前被操作的 ChartView
        QChartView *currentChartView = qobject_cast<QChartView*>(watched);
        QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis*>(currentChartView->chart()->axes(Qt::Horizontal).at(0));
        if (!axisX) return false;

        // 获取当前时间范围
        QDateTime min = axisX->min();
        QDateTime max = axisX->max();
        qint64 totalSecs = min.secsTo(max);

        // 自动调整时间格式
        if (totalSecs < 3600) {
            axisX->setFormat("HH:mm:ss");
        } else if (totalSecs < 86400) {
            axisX->setFormat("MM-dd HH:mm");
        } else {
            axisX->setFormat("yyyy-MM-dd");
        }

        // 计算缩放比例（滚轮向上=放大，向下=缩小）
        double scaleFactor = wheelEvent->angleDelta().y() > 0 ? 0.9 : 1.1;
        qint64 newSecs = totalSecs * scaleFactor;

        // 限制缩放范围（1分钟~7天）
        qint64 minSecs = 60;
        qint64 maxSecs = 7 * 86400;
        newSecs = qBound(minSecs, newSecs, maxSecs);

        // 以鼠标位置为中心计算新范围
        QPointF mousePos = wheelEvent->position();
        double mouseRatio = mousePos.x() / currentChartView->width(); // 鼠标在图表中的X比例
        QDateTime mouseDateTime = min.addSecs(static_cast<qint64>(totalSecs * mouseRatio));

        QDateTime newMin = mouseDateTime.addSecs(-newSecs * mouseRatio);
        QDateTime newMax = mouseDateTime.addSecs(newSecs * (1 - mouseRatio));

        // 更新X轴范围实现缩放
        axisX->setRange(newMin, newMax);

        // 阻止事件传递（避免滚动穿透）
        return true;
        }
    return QWidget::eventFilter(watched, event);
}

//查询按钮
void test::on_pushQuery_clicked() {
    updateChartData(true); // 调用更新图表数据
}

//重置按钮
void test::on_pushClear_clicked() {
    ui->dateStartTime->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->dateOverTime->setDateTime(QDateTime::currentDateTime());
    // 统一重置函数：安全处理单个 ChartView
    auto resetChartXAxis = [&](QChartView *view) {
        if (!view || !view->chart()) return;
        auto axes = view->chart()->axes(Qt::Horizontal);
        if (axes.isEmpty()) return;
        QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis*>(axes.at(0));
        if (axisX) {
            axisX->setRange(ui->dateStartTime->dateTime(), ui->dateOverTime->dateTime());
            axisX->setFormat("yyyy-MM-dd");
        }
        view->chart()->zoomReset(); // 同时重置缩放
    };
    // 一键重置所有图表
    resetChartXAxis(airChartView);
    resetChartXAxis(lightChartView);
    resetChartXAxis(soilChartView);
}

void test::on_pushClearHistory_clicked() {
    QMessageBox::StandardButton result=QMessageBox::question(this,
        "删除确认","是否删除所选时间范围的历史记录？",
        QMessageBox::Yes |QMessageBox::No,
        QMessageBox::No);
    if (result !=QMessageBox::Yes) {
        return;
    }
    QDateTime start = ui->dateStartTime->dateTime();
    QDateTime end = ui->dateOverTime->dateTime();

    if (start>end) {
        QMessageBox::warning(this,"警告","起始时间不能比结束时间晚");
        return;
    }
    std::string startTime=start.toString("yyyy-MM-dd HH:mm").toStdString();
    std::string endTime=end.toString("yyyy-MM-dd HH:mm").toStdString();
    bool deleteSuc = Database::deleteByTime(startTime, endTime);
        if (deleteSuc) {
            QMessageBox::information(this,"删除成功","历史记录已经删除！");
        }else{
            QMessageBox::critical(this,"删除失败","请重试");
    }
}
