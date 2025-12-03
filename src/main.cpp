#include <iostream>

#include "widget\\MainWindow\\mainwindow.h"
#include "sqlite_orm.h"
#include "Database/Database.h"
#include <QApplication>
#include "HistoryData/test.h"
using namespace sqlite_orm;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();
    test te;
    te.show();
    return a.exec();

}
