#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QLabel>
#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

private slots:
    void updateEnvironmentData();
    void onRefreshButtonClicked();

private:
    Ui::HomePage *ui;
    QTimer *m_updateTimer;

    void loadStyleSheet();
    void updateSuggestion();
    void updateCardStatus(QLabel* statusLabel, const QString& status);
};

#endif // HOMEPAGE_H
