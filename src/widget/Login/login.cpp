#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    //跳转到实时数据采集页面的槽函数链接
    connect(ui->pbtLogin,&QPushButton::clicked,this,&Login::showRealTimeDate);
}

Login::~Login()
{
    delete ui;
}


void Login::on_pbtLogin_clicked()
{
    Name = ui->letName->text();
    Passward = ui->letPassWrod->text();

    QMessageBox::information(this,"提示","登陆成功！");
}


void Login::on_pbtLoginReset_clicked()
{
    ui->letName->clear();
    ui->letPassWrod->clear();
}


void Login::on_pbtRegister_clicked()
{
    Name = ui->letNewName->text();
    Passward = ui->letNewPassward->text();
    ComfirmPassward = ui->letComfirmPassward->text();

    if(Name == NULL){
        QMessageBox::warning(this,"警告","用户名不能空");
        return;
    }
    if(Passward != ComfirmPassward){
        QMessageBox::warning(this,"警告","请保证两次输入密码相同！");
        return;
    }
    QMessageBox::information(this,"提示","注册成功，请返回登陆！");
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
