#include "widget\\MainWindow\\mainwindow.h"
#include "sqlite_orm.h"

#include <QApplication>

#include "widget/RealTimeDate/realtimedate.h"
using namespace sqlite_orm;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    RealTimeDate r;
    r.show();


    return a.exec();
}
