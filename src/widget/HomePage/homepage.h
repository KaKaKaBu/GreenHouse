#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QLabel>
#include <QWidget>
#include <QTimer>

#include "viewmodel/SensorViewModel.h"
#include "viewmodel/SerialViewModel.h"
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

public slots:
    void updateEnvironmentData(const SensorRecord& data);

private:
    Ui::HomePage *ui;
    QTimer *m_updateTimer;

    void loadStyleSheet();
    void updateSuggestion(const SensorRecord& data);
    void updateCardStatus(QLabel* statusLabel, const QString& status);

};

#endif // HOMEPAGE_H
