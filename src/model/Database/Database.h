#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <string>
#include "model/SensorData.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <vector>
#include "sqlite_orm.h"
class Database {
public:
    static Database& instance();
    auto& getStorage();
    bool insert(const SensorRecord& data);//插入
    //查询指定时间范围的数据
    bool queryByTime(const std::string& startTime,const std::string& endTime,std::vector<SensorRecord>& outResults);
    bool deleteByTime(const std::string& startTime,const std::string&  endTime);
private:
    Database();
    ~Database()=default;
    decltype(sqlite_orm::make_storage(
       std::string{},  // 占位符，实际类型由 .cpp 中的初始化决定
       sqlite_orm::make_table("green_data",
           sqlite_orm::make_column("id", &SensorRecord::id, sqlite_orm::primary_key().autoincrement()),
           sqlite_orm::make_column("record_time", &SensorRecord::record_time),
           sqlite_orm::make_column("air_temp", &SensorRecord::air_temp),
           sqlite_orm::make_column("air_humid", &SensorRecord::air_humid),
           sqlite_orm::make_column("soil_humid", &SensorRecord::soil_humid),
           sqlite_orm::make_column("light_intensity", &SensorRecord::light_intensity)
       )
   )) m_storage;  // 类型由 decltype 推断e;
};

#endif // DATABASE_H