#include "login.h"
#include "ui_login.h"
#include "../../model/PersonDatabase/CurrentUser.h"
#include  "model/PersonDatabase/Person.h"
#include "widget/UserInfo/userinfo.h"
#include <QDebug>
Login::Login(QWidget *parent)
    : QWidget(parent),person("green-house.db"), ui(new Ui::Login)
{
    ui->setupUi(this);
    // //跳转到实时数据采集页面的槽函数链接
    // connect(ui->pbtLogin,&QPushButton::clicked,this,&Login::showRealTimeDate);
}

Login::~Login()
{
    delete ui;
}


void Login::on_pbtLogin_clicked()
{
    QString username = ui->letName->text().trimmed();
    QString password = ui->letPassWrod->text().trimmed();
    
    // 验证输入是否为空
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和密码不能为空！");
        return;
    }
    
    // 验证用户信息
    if (person.verifyUser(username.toStdString(), password.toStdString())) {

        QMessageBox::information(this, "提示", "登陆成功！");

        int userId = person.getUserIdByUsername(username.toStdString());
        CurrentUser::id = userId;
        // UserInfo* userInfo = new UserInfo();
        // userInfo->show();
        // 保存用户名
        Name = username;
        Passward = password;
        qDebug() << "[Login] 设置 CurrentUser::id 为:" << CurrentUser::id;
        // 发送登录成功信号，通知 MainWindow 更新状态
       emit loginSuccess();
        emit snedUserInfo(userId,Name,Passward);

    } else {
        QMessageBox::warning(this, "错误", "用户名或密码错误！");
        ui->letName->clear();
        ui->letPassWrod->clear();
    }
}


void Login::on_pbtLoginReset_clicked()
{
    ui->letName->clear();
    ui->letPassWrod->clear();
}


void Login::on_pbtRegister_clicked()
{
    QString username = ui->letNewName->text().trimmed();
    QString password = ui->letNewPassward->text();
    QString ComfirmPassward = ui->letComfirmPassward->text();
    if(username.isEmpty()){
        QMessageBox::warning(this,"警告","用户名不能空");
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "警告", "密码不能为空！");
        return;
    }
    if (password != ComfirmPassward) {
        QMessageBox::warning(this, "警告", "两次输入的密码不一致！");
        return;
    }
    if(password != ComfirmPassward){
        QMessageBox::warning(this,"警告","请保证两次输入密码相同！");
        return;
    }
    int result=person.insertPerson(username.toStdString(),password.toStdString());
    if (result>0) {
        QMessageBox::information(this,"成功","注册成功,请返回登录！");
        ui->letNewName->clear();
        ui->letNewPassward->clear();
        ui->letComfirmPassward->clear();
    }else if (result==-1){
        QMessageBox::warning(this,"警告",QString("用户名\"%1\"已存在，请重新输入").arg(Name));
    }
}


void Login::on_pbtRegisterReset_clicked()
{
    ui->letNewName->clear();
    ui->letNewPassward->clear();
    ui->letComfirmPassward->clear();
}

void Login::showRealTimeDate()
{
    realtimedate = new RealTimeDate;
    realtimedate->show();

    this->hide();

    connect(realtimedate,&RealTimeDate::close,this,&QWidget::show);
}
