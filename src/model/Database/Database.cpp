#include "Database.h"
#include "sqlite_orm.h"
#include <QDebug>
using namespace std;
bool Database::insert(const GreenData &data) {
    try {
        m_storage.insert(data);
        return true;
    }catch (const std::exception& e) {
        std::cerr<<"插入失败"<<e.what()<<std::endl;
        return false;
    }
}

bool Database::queryByTime(const std::string &startTime, const std::string &endTime,std::vector<GreenData>& outResults) {
    try {
               outResults = m_storage.get_all<GreenData>(
                    sqlite_orm::where(
                       sqlite_orm::between(&GreenData::record_time, startTime, endTime)
                    )
                );
               return true; // 成功
            } catch (const std::exception& e) {
                    qDebug()<< "数据库查询异常:" << e.what();
                   outResults.clear(); // 确保输出是干净的
                    return false;       // 失败
               }
}

bool Database::deleteByTime(const std::string &startTime, const std::string &endTime) {
    try {
        m_storage.remove_all<GreenData>(
             sqlite_orm::where(
                 sqlite_orm::between(&GreenData::record_time,startTime,endTime)
        ));
        std::cout<<"删除成功"<<endl;
        return true;
    }catch (const std::exception& e) {
        std::cerr<<"按时间删除失败"<<e.what()<<endl;
        return false;
    }
}

