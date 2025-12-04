#include "widget\\MainWindow\\mainwindow.h"
#include "sqlite_orm.h"

#include <QApplication>
#include "../src/widget/Login/Login.h"
#include "../src/widget/UserInfo/userinfo.h"

#include "Toast.h"
#include "widget/HistoryData/test.h"
#include "widget/RealTimeDate/realtimedate.h"
#include "widget/HistoryData/test.h"
#include "widget/Login/login.h"
using namespace sqlite_orm;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    // RealTimeDate r;
    // r.show();
    // test test;
    // test.show();
    // Toast::setPosition(ToastPosition::BOTTOM_RIGHT);
    // Toast::setSpacing(20);
    // Toast::setMaximumOnScreen(5);
    // Login login;
    // login.show();
    UserInfo user;
    user.show();
    return a.exec();
}
