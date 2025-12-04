#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include "../RealTimeDate/realtimedate.h"
#include "model/PersonDatabase/Person.h"
namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

signals:
    // 登录成功信号
    void loginSuccess();
    // 登出信号
    void logout();

private slots:
    void on_pbtLogin_clicked();

    void on_pbtLoginReset_clicked();

    void on_pbtRegister_clicked();

    void on_pbtRegisterReset_clicked();

    //定义跳转到实时采集界面的槽函数（已废弃，改用信号）
    void showRealTimeDate();


private:
    Ui::Login *ui;
    Person person;
    //定义用户名密码
    QString  Name;
    QString  Passward;
    QString  ComfirmPassward;
    //创建实时采集页面对象（已废弃）
    RealTimeDate *realtimedate;
};

#endif // LOGIN_H
