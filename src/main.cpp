#include "widget\\MainWindow\\mainwindow.h"
#include "third-party/sqlite_orm/sqlite_orm.h"

#include <QApplication>
using namespace sqlite_orm;
int main(int argc, char *argv[])
{
    struct Envdata
    {
        int id;
        double temp;
    };
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    auto storage=make_storage("greenhouse.db",
                                make_table("envData",
                                           make_column("id",&Envdata::id,primary_key().autoincrement()),
                                           make_column("tmp",&Envdata::temp,not_null())));
    storage.sync_schema();
    return a.exec();
}
