#include "widget\\MainWindow\\mainwindow.h"
#include "sqlite_orm.h"

#include <QApplication>
#include "../src/widget/Login/Login.h"
#include "../src/widget/UserInfo/userinfo.h"

#include "widget/RealTimeDate/realtimedate.h"
#include "widget/HistoryData/test.h"
using namespace sqlite_orm;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    // RealTimeDate r;
    // r.show();

    // Login l;
    // l.show();

    UserInfo u;
    u.show();

    test tes;
    tes.show();
    return a.exec();
}
