#include "widget\\MainWindow\\mainwindow.h"
#include "sqlite_orm.h"

#include <QApplication>

#include "widget/HistoryData/test.h"
using namespace sqlite_orm;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();
    test tes;
    tes.show();
    return a.exec();
}
