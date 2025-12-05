#include "userinfo.h"
#include "ui_UserInfo.h"
#include "widget/Login/login.h" // 引入 Login 头文件（根据实际路径调整）
#include "widget/MainWindow/mainwindow.h"

UserInfo::UserInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserInfo)
    , login(nullptr)
    , m_currentUserId(-1)
    , m_personDB(new Person("./green-house.db"))

{
    ui->setupUi(this);

    // 默认配置：输入框只读、保存按钮禁用
    ui->userName->setReadOnly(true);
    ui->userPassward->setReadOnly(true);
    ui->pbtSave->setEnabled(false);

    // 绑定注销按钮槽函数
    connect(ui->pbtCancel, &QPushButton::clicked, this, &UserInfo::backToLogin);

}

UserInfo::~UserInfo()
{
    delete m_personDB;
    delete ui;
}

// 核心方法：接收用户信息并立即显示到输入框
void UserInfo::setCurrentUser(int userId, const QString &username, const QString &password)
{
    // 1. 保存用户信息到本地变量
    m_currentUserId = userId;
    m_currentUsername = username;
    m_currentPassword = password;

    // 2. 立即显示到页面输入框（这一步实现“打开即显示”）
    ui->userName->setText(username);       // 显示用户名
    ui->userPassward->setText(password);   // 显示密码
}

void UserInfo::on_pbtchange_clicked()
{
    // 点击修改：开启编辑权限
    ui->userName->setReadOnly(false);
    ui->userPassward->setReadOnly(false);
    ui->pbtSave->setEnabled(true);
    ui->userName->setFocus();
}

void UserInfo::on_pbtSave_clicked()
{
    if (!ui->pbtSave->isEnabled()) {
        qDebug() << "无法保存：按钮未激活";
        return;
    }

    QString newUsername = ui->userName->text().trimmed();
    QString newPassword = ui->userPassward->text().trimmed();

    // 非空校验
    if (newUsername.isEmpty() || newPassword.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和密码不能为空！");
        return;
    }

    // 用户名重复校验（仅修改用户名时）
    if (newUsername != m_currentUsername) {
        int existUserId = m_personDB->getUserIdByUsername(newUsername.toStdString());
        if (existUserId != -1 && existUserId != m_currentUserId) {
            QMessageBox::warning(this, "警告", "用户名已存在，请更换！");
            ui->userName->setText(m_currentUsername);
            return;
        }
    }

    // 调用数据库修改
    bool updateOk = m_personDB->updatePerson(
        m_currentUserId,
        newUsername.toStdString(),
        newPassword.toStdString()
    );

    if (updateOk) {
        QMessageBox::information(this, "提示", "修改已保存！");
        m_currentUsername = newUsername;
        m_currentPassword = newPassword;
    } else {
        QMessageBox::critical(this, "错误", "修改失败，请重试！");
        ui->userName->setText(m_currentUsername);
        ui->userPassward->setText(m_currentPassword);
    }

    // 恢复只读状态
    ui->userName->setReadOnly(true);
    ui->userPassward->setReadOnly(true);
    ui->pbtSave->setEnabled(false);
}

void UserInfo::on_pbtCancel_clicked()
{
    int ret = QMessageBox::question(this, "确认", "是否确定注销？",
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;
    bool deleteOK=m_personDB->deletePerson(m_currentUserId);
    if (deleteOK) {
          ui->userName->clear();
    ui->userPassward->clear();
    QMessageBox::information(this, "提示", "注销成功，即将返回登录页！");
    }

}

void UserInfo::backToLogin()
{
    // if (!login) login = new Login;
    // login->show();
    // this->hide();
    emit logOut(false);
}