//
// Created by Trubson on 2025/12/4.
//

#ifndef GREENHOUSEAPP_USERINFO_H
#define GREENHOUSEAPP_USERINFO_H

#include <QWidget>

#include <QMessageBox>
#include <QDebug>

#include "../Login/login.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class UserInfo;
}

QT_END_NAMESPACE

class UserInfo : public QWidget {
    Q_OBJECT

public:
    explicit UserInfo(QWidget *parent = nullptr);

    ~UserInfo() override;

private slots:
    void on_pbtchange_clicked();

    void on_pbtSave_clicked();

    void on_pbtCancel_clicked();

    //注销账号后跳转到登录页面
    void backToLogin();

private:
    Ui::UserInfo *ui;

    Login * login;
};


#endif //GREENHOUSEAPP_USERINFO_H