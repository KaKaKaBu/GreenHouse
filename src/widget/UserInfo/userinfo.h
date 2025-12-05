#ifndef GREENHOUSEAPP_USERINFO_H
#define GREENHOUSEAPP_USERINFO_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>
// 引入 Person 数据库操作类（根据你的实际路径调整）
#include "model/PersonDatabase//Person.h"
#include "widget/MainWindow/mainwindow.h"


// 前置声明 Login 类
class Login;

QT_BEGIN_NAMESPACE
namespace Ui { class UserInfo; }
QT_END_NAMESPACE

class UserInfo : public QWidget {
    Q_OBJECT

public:
    explicit UserInfo(QWidget *parent = nullptr);
    ~UserInfo() override;

    // 核心接口：设置当前登录用户信息（供 Login 页面调用）
    // 调用这个接口后，页面会立即显示传入的用户名和密码
    void setCurrentUser(int userId, const QString &username, const QString &password);
 signals:
    void logOut(bool index);


private slots:
    void on_pbtchange_clicked();
    void on_pbtSave_clicked();
    void on_pbtCancel_clicked();
    void backToLogin();

private:
    Ui::UserInfo *ui;
    Login *login;

    // 保存当前登录用户信息
    int m_currentUserId;          // 当前用户ID
    QString m_currentUsername;    // 当前用户名
    QString m_currentPassword;    // 当前密码
    Person *m_personDB;           // 数据库操作对象
};

#endif // GREENHOUSEAPP_USERINFO_H