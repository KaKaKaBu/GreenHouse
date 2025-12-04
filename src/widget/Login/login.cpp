#include "login.h"
#include "ui_login.h"
#include  "model/PersonDatabase/Person.h"
Login::Login(QWidget *parent)
    : QWidget(parent)
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
    if (Name.isEmpty() || Passward.isEmpty()) {
        QMessageBox::warning(this,"警告","用户名和密码不能为空！");

    }
    if (Person::queryPerson(Name.toStdString(),Passward.toStdString())) {
        QMessageBox::information(this,"提示","登陆成功！");
    }

}


void Login::on_pbtLoginReset_clicked()
{
    ui->letName->clear();
    ui->letPassWrod->clear();
}


void Login::on_pbtRegister_clicked()
{
    Name = ui->letNewName->text().trimmed();
    Passward = ui->letNewPassward->text();
    ComfirmPassward = ui->letComfirmPassward->text();
    if(Name.isEmpty()){
        QMessageBox::warning(this,"警告","用户名不能空");
        return;
    }
    if(Passward != ComfirmPassward){
        QMessageBox::warning(this,"警告","请保证两次输入密码相同！");
        return;
    }
    Persons newPersons;
    newPersons.username=Name.toStdString();
    newPersons.password=Passward.toStdString();
    int result = Person::insertPerson(newPersons);
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
