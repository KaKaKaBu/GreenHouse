#include "Database.h"
#include <iostream>
#include <QDebug>


// 静态辅助函数：创建存储（用于初始化 m_storage）
static auto makeStorage() {
    return sqlite_orm::make_storage("green-house.db",
        sqlite_orm::make_table("green_data",
            sqlite_orm::make_column("id", &SensorRecord::id, sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("record_time", &SensorRecord::record_time),
            sqlite_orm::make_column("air_temp", &SensorRecord::air_temp),
            sqlite_orm::make_column("air_humid", &SensorRecord::air_humid),
            sqlite_orm::make_column("soil_humid", &SensorRecord::soil_humid),
            sqlite_orm::make_column("light_intensity", &SensorRecord::light_intensity)
        )
    );
}

Database::Database()
    : m_storage(makeStorage())  // 实际初始化
{
    m_storage.sync_schema();
    std::cout << "数据库初始化成功" << std::endl;
}

Database& Database::instance() {
    static Database db;
    return db;
}

auto& Database::getStorage() {
    return m_storage;
}
bool Database::insert(const SensorRecord &data) {
    try {
        m_storage.insert(data);
        return true;
    }catch (const std::exception& e) {
        std::cerr<<"插入失败"<<e.what()<<std::endl;
        return false;
    }
}

bool Database::queryByTime(const std::string &startTime, const std::string &endTime,std::vector<SensorRecord>& outResults) {
    try {
        outResults = m_storage.get_all<SensorRecord>(
             sqlite_orm::where(
                sqlite_orm::between(&SensorRecord::record_time, startTime, endTime)
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
        m_storage.remove_all<SensorRecord>(
             sqlite_orm::where(
                 sqlite_orm::between(&SensorRecord::record_time,startTime,endTime)
        ));
        std::cout<<"删除成功"<<std::endl;
        return true;
    }catch (const std::exception& e) {
        std::cerr<<"按时间删除失败"<<e.what()<<std::endl;
        return false;
    }
}