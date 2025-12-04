//
// Created by Trubson on 2025/12/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_UserInfo.h" resolved

#include "userinfo.h"
#include "ui_UserInfo.h"


UserInfo::UserInfo(QWidget *parent) : QWidget(parent), ui(new Ui::UserInfo) {
    ui->setupUi(this);

    //默认用户信息框无法点击修改
    ui->userName->setReadOnly(true);
    ui->userPassward->setReadOnly(true);
    //默认保存按钮也无法点击
    ui->pbtSave->setDefault(false);

    //注销后返回登录界面的槽函数链接
    connect(ui->pbtCancel,&QPushButton::clicked,this,&UserInfo::backToLogin);
}

UserInfo::~UserInfo() {
    delete ui;
}

void UserInfo::on_pbtchange_clicked()
{
    ui->userName->setReadOnly(false);
    ui->userPassward->setReadOnly(false);
    ui->pbtSave->setDefault(true);
}


void UserInfo::on_pbtSave_clicked()
{
    if(!ui->pbtSave->isDefault()){
        qDebug() <<"无法保存";
        return;
    }

    QMessageBox::information(this,"提示","修改已保存！");
    ui->userName->setReadOnly(true);
    ui->userPassward->setReadOnly(true);

    ui->pbtSave->setDefault(false);
}


void UserInfo::on_pbtCancel_clicked()
{
    ui->userName->clear();
    ui->userPassward->clear();

    //调用封装的删除函数删除数据库对应的内容

    QMessageBox::information(this,"提示","注销成功，该界面消失后返回登陆页面！");
}

void UserInfo::backToLogin()
{
    login = new Login;
    login->show();

    this->hide();
}